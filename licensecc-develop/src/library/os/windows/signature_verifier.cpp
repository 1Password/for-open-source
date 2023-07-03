/*
 * verifier.cpp
 *
 *  Created on: Nov 16, 2019
 *      Author: devel
 */

#include "../os.h"
#include <stdio.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <bcrypt.h>
#include <wincrypt.h>
#include <iphlpapi.h>
#include <windows.h>
//#pragma comment(lib, "bcrypt.lib")

#include <public_key.h>
#include "../../base/logger.h"
#include "../../base/base64.h"
#include "../signature_verifier.hpp"

#define RSA_KEY_BITLEN 1024

namespace license {
namespace os {
using namespace std;
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

static const void formatError(DWORD status, const char *description) {
	char msgBuffer[256];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, status, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), &msgBuffer[0],
				  sizeof(msgBuffer) - 1, nullptr);
	LOG_DEBUG("error %s : %s %h", description, msgBuffer, status);
}

#pragma pack(push, 1)
typedef struct {
	BCRYPT_RSAKEY_BLOB rsakey;
	BYTE pkExp[3];
	BYTE modulus[RSA_KEY_BITLEN / 8];
} PUBKEY_BLOB, *P_PUBKEY_BLOB;
#pragma pack(pop)

static BCRYPT_ALG_HANDLE openHashProvider() {
	DWORD status;
	BCRYPT_ALG_HANDLE hHashAlg = nullptr;
	if (!NT_SUCCESS(status = BCryptOpenAlgorithmProvider(&hHashAlg, BCRYPT_SHA256_ALGORITHM, NULL, 0))) {
		throw logic_error("Error opening hash provider");
	}
	return hHashAlg;
}

static DWORD hashData(BCRYPT_HASH_HANDLE& hHash, const string& data, PBYTE pbHash, DWORD hashDataLenght) {
	DWORD status;
	if (NT_SUCCESS(status = BCryptHashData(hHash, (BYTE*)data.c_str(), (ULONG)data.length(), 0))) {
		status = BCryptFinishHash(hHash, pbHash, hashDataLenght, 0);
	}
	return status;
}

static size_t read_length(uint8_t*& ptr) {
	uint8_t len = *ptr++;
	size_t result = 0;
	cout << (len & 0x80) << endl;
	if ((len & 0x80) > 0) {
		size_t blen = len & 0x7F;
		for (size_t i = 0; i < blen; i++) {
			result += (*(ptr++) << (i * 8));
		}
	} else {
		result = len;
	}
	return result;
}

static FUNCTION_RETURN read_sequence(uint8_t*& ptr) {
	uint8_t tag = *ptr++;
	if (tag != 0x30) {
		return FUNC_RET_ERROR;
	}
	read_length(ptr);
	return FUNC_RET_OK;
}

static FUNCTION_RETURN read_integer(uint8_t*& ptr, BYTE* location, const size_t expected_length) {
	uint8_t tag = *ptr++;
	if (tag != 0x02) {
		return FUNC_RET_ERROR;
	}
	size_t length = read_length(ptr);
	// skip the padding byte
	if (*ptr == 0) {
		length--;
		ptr++;
	}
	if (expected_length < length) {
		return FUNC_RET_ERROR;
	}
	for (size_t i = 0; i < length; i++) {
		location[i] = *(ptr++);
	}
	return FUNC_RET_OK;
}

static FUNCTION_RETURN readPublicKey(const BCRYPT_ALG_HANDLE sig_alg, BCRYPT_KEY_HANDLE* hKey) {
	FUNCTION_RETURN result = FUNC_RET_ERROR;
	DWORD status;
	PUBKEY_BLOB pubk;
	pubk.rsakey.Magic = BCRYPT_RSAPUBLIC_MAGIC;
	pubk.rsakey.BitLength = RSA_KEY_BITLEN;
	pubk.rsakey.cbPublicExp = 3;
	pubk.rsakey.cbModulus = RSA_KEY_BITLEN / 8;
	pubk.rsakey.cbPrime1 = 0;
	pubk.rsakey.cbPrime2 = 0;
	uint8_t pubKey[] = PUBLIC_KEY;
	uint8_t* pub_key_idx = &pubKey[0];
	read_sequence(pub_key_idx);
	read_integer(pub_key_idx, (BYTE*)&pubk.modulus, sizeof(pubk.modulus));
	read_integer(pub_key_idx, (BYTE*)&pubk.pkExp, sizeof(pubk.pkExp));
	if (NT_SUCCESS(status = BCryptImportKeyPair(sig_alg, nullptr, BCRYPT_RSAPUBLIC_BLOB, hKey, (PUCHAR)&pubk,
												sizeof(pubk), 0))) {
		result = FUNC_RET_OK;
	} else {
#ifndef NDEBUG
		formatError(status, "error importing public key");
#endif
	}
	return result;
}

static FUNCTION_RETURN verifyHash(const PBYTE pbHash, const DWORD hashDataLenght, const string& signatureBuffer) {
	BCRYPT_KEY_HANDLE phKey = nullptr;
	DWORD status;
	FUNCTION_RETURN result = FUNC_RET_ERROR;
	PBYTE pbSignature = nullptr;
	BCRYPT_ALG_HANDLE hSignAlg = nullptr;

	vector<uint8_t> signatureBlob = unbase64(signatureBuffer);
	DWORD dwSigLen = (DWORD) signatureBlob.size();
	BYTE* sigBlob = &signatureBlob[0];

	if (NT_SUCCESS(status = BCryptOpenAlgorithmProvider(&hSignAlg, BCRYPT_RSA_ALGORITHM, NULL, 0))) {
		if ((result = readPublicKey(hSignAlg, &phKey)) == FUNC_RET_OK) {
			BCRYPT_PKCS1_PADDING_INFO paddingInfo;
			ZeroMemory(&paddingInfo, sizeof(paddingInfo));
			paddingInfo.pszAlgId = BCRYPT_SHA256_ALGORITHM;
			if (NT_SUCCESS(status = BCryptVerifySignature(phKey, &paddingInfo, pbHash, hashDataLenght, sigBlob,
														  dwSigLen, BCRYPT_PAD_PKCS1))) {
				result = FUNC_RET_OK;
			} else {
				result = FUNC_RET_ERROR;
#ifndef NDEBUG
				formatError(status, "error verifying signature");
#endif
			}
		} else {
			LOG_DEBUG("Error reading public key");
		}
	}
	else {
		result = FUNC_RET_NOT_AVAIL;
#ifndef NDEBUG
		formatError(status, "error opening RSA provider");
#endif
	}

	if (phKey != nullptr) {
		BCryptDestroyKey(phKey);
	}
	if (hSignAlg != nullptr) {
		BCryptCloseAlgorithmProvider(hSignAlg, 0);
	}
	//if (sigBlob) {
	//	free(sigBlob);
	//}
	return result;
}

FUNCTION_RETURN verify_signature(const std::string& stringToVerify, const std::string& signatureB64) {
	BCRYPT_HASH_HANDLE hHash = nullptr;
	PBYTE pbHashObject = nullptr, pbHashData = nullptr;

	FUNCTION_RETURN result = FUNC_RET_ERROR;
	const HANDLE hProcessHeap = GetProcessHeap();
	// BCRYPT_ALG_HANDLE sig_alg = openSignatureProvider();

	BCRYPT_ALG_HANDLE hash_alg = openHashProvider();
	DWORD status;

	// calculate the size of the buffer to hold the hash object
	DWORD cbData = 0, cbHashObject = 0;
	// and the size to keep the hashed data
	DWORD cbHashDataLenght = 0;
	if (NT_SUCCESS(status = BCryptGetProperty(hash_alg, BCRYPT_OBJECT_LENGTH, (PBYTE)&cbHashObject, sizeof(DWORD),
											  &cbData, 0)) &&
		NT_SUCCESS(status = BCryptGetProperty(hash_alg, BCRYPT_HASH_LENGTH, (PBYTE)&cbHashDataLenght, sizeof(DWORD),
											  &cbData, 0))) {
		// allocate the hash object on the heap
		pbHashObject = (PBYTE)HeapAlloc(hProcessHeap, 0, cbHashObject);
		pbHashData = (PBYTE)HeapAlloc(hProcessHeap, 0, cbHashDataLenght);
		if (NULL != pbHashObject && nullptr != pbHashData) {
			if (NT_SUCCESS(status = BCryptCreateHash(hash_alg, &hHash, pbHashObject, cbHashObject, NULL, 0, 0))) {
				if (NT_SUCCESS(status = hashData(hHash, stringToVerify, pbHashData, cbHashDataLenght))) {
					result = verifyHash(pbHashData, cbHashDataLenght, signatureB64);
				} else {
					result = FUNC_RET_NOT_AVAIL;
#ifndef NDEBUG
					formatError(status, "error hashing data");
#endif
				}
			} else {
				result = FUNC_RET_NOT_AVAIL;
#ifndef NDEBUG
				formatError(status, "error creating hash");
#endif
			}
		} else {
			result = FUNC_RET_BUFFER_TOO_SMALL;
			LOG_DEBUG("Error allocating memory");
		}
	} else {
		result = FUNC_RET_NOT_AVAIL;
#ifndef NDEBUG
		formatError(status, "**** Error returned by BCryptGetProperty");
#endif
	}

	if (hHash) {
		BCryptDestroyHash(hHash);
	}
	if (pbHashObject) {
		HeapFree(hProcessHeap, 0, pbHashObject);
	}
	if (pbHashData) {
		HeapFree(hProcessHeap, 0, pbHashData);
	}
	if (hash_alg != nullptr) {
		BCryptCloseAlgorithmProvider(hash_alg, 0);
	}
	return result;
}
}  // namespace os
} /* namespace license */
