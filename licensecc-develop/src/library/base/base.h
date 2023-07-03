#ifndef BASE_H_
#define BASE_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __unix__
#include <limits.h>
#define DllExport
#ifndef MAX_PATH
#define MAX_PATH PATH_MAX
#endif

#else  // windows
#include <windows.h>
#define DllExport __declspec(dllexport)

#ifndef __cplusplus
#ifndef _MSC_VER
#include <stdbool.h>
#else
typedef int bool;
#define false 0
#define true - 1
#endif
#endif

#endif

// license file parameters
#define PARAM_EXPIRY_DATE "valid-to"
#define PARAM_BEGIN_DATE "valid-from"
#define PARAM_VERSION_FROM "start-version"
#define PARAM_CLIENT_SIGNATURE "client-signature"
#define PARAM_VERSION_TO "end-version"
#define PARAM_EXTRA_DATA "extra-data"
// license file extra entries
#define LICENSE_SIGNATURE "sig"
#define LICENSE_VERSION "lic_ver"

typedef enum { FUNC_RET_OK, FUNC_RET_NOT_AVAIL, FUNC_RET_ERROR, FUNC_RET_BUFFER_TOO_SMALL } FUNCTION_RETURN;

#define cmin(a, b)              \
	({                          \
		__typeof__(a) _a = (a); \
		__typeof__(b) _b = (b); \
		_a < _b ? _a : _b;      \
	})

#ifdef __cplusplus
}
#endif

#endif
