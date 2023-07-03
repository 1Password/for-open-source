/*********************  physical_processors.cpp   *****************************
* Author:        Agner Fog
* Date created:  2019-10-29
* Last modified: 2019-11-25
* Version:       1.02 beta
* Project:       vector class library
* Description:   Detect number of physical and logical processors on CPU chip.
*                Compile for C++11 or later
*
* (c) Copyright 2019 Agner Fog.
* Apache License version 2.0 or later.
*******************************************************************************
Some modern CPUs can run two threads in each CPU core when simultaneous 
multithreading (SMT, called hyperthreading by Intel) is enabled.

The number of physical processors is the number of CPU cores.
The number of logical processors is the same number multiplied by the number of
threads that can run simultaneously in each CPU core.

Simultaneous multithreading will slow down performance when two CPU-intensive 
threads running in the same physical processor (CPU core) are competing for the
same resources. Therefore, the optimal number of threads for CPU-intensive
tasks is most likely to be the number of physical processors. 

Tasks that are less CPU-intensive but limited by RAM access, disk access, 
network, etc. may get an advantage by running as many threads as the number of
logical processors. This will be double the number of physical processors when
simultaneous multithreading is enabled.

The physicalProcessors function detects the number of physical processors and
logical processors on an x86 computer. This is useful for determining the 
optimal number of threads.


Note: There are several problems in detecting the number of physical processors:

1. The CPUID instruction on Intel CPUs will return a wrong number of logical
   processors when SMT (hyperthreading) is disabled. It may be necessary to 
   compare the number of processors returned by the CPUID instruction with the
   number of processors reported by the operating system to detect if SMT is 
   enabled (AMD processors do not have this problem).

2. It is necessary to rely on system functions to detect if there is more than 
   one CPU chip installed. It is assumed that the status of SMT is the same on
   all CPU chips in a system.

3. The behavior of VIA processors is undocumented.
   
4. This function is not guaranteed to work on future CPUs. It may need updating
   when new CPUs with different configurations or different CPUID functionality
   appear.
******************************************************************************/

#include <thread>     // std::thread functions

#ifdef _MSC_VER
#include <intrin.h>   // __cpuidex intrinsic function available on microsoft compilers
#endif

// Define interface to CPUID instruction.
// input:  leaf = eax, subleaf = ecx
// output: output[0] = eax, output[1] = ebx, output[2] = ecx, output[3] = edx
static inline void cpuid(int output[4], int leaf, int subleaf = 0) {
#if defined(__GNUC__) || defined(__clang__)      // use inline assembly, Gnu/AT&T syntax
    int a, b, c, d;
    __asm("cpuid" : "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "a"(leaf), "c"(subleaf) : );
    output[0] = a;
    output[1] = b;
    output[2] = c;
    output[3] = d;

#elif defined (_MSC_VER)                         // Microsoft compiler, intrin.h included
    __cpuidex(output, leaf, subleaf);            // intrinsic function for CPUID

#else                                            // unknown platform. try inline assembly with masm/intel syntax
    __asm {
        mov eax, leaf
        mov ecx, subleaf
        cpuid;
        mov esi, output
        mov[esi], eax
        mov[esi + 4], ebx
        mov[esi + 8], ecx
        mov[esi + 12], edx
    }
#endif
}

// Function prototype:
int physicalProcessors(int * logical_processors = 0);


// Find the number of physical and logical processors supported by CPU
// Parameter: 
// logical_processors: an optional pointer to an integer that will receive the number of logical processors.
// Return value: number of physical processors
int physicalProcessors(int * logical_processors) {
    int vendor = 0;                              // CPU vendor: 1 = Intel, 2 = AMD, 3 = VIA, 0 = other
    int logicalProc = 1;                         // number of logical processor cores
    int physicalProc = 1;                        // number of physical processor cores
    int procPerCore = 1;                         // logical cores per physical core
    bool hyperthreadingSupported = false;        // CPU supports hyperthreading / simultaneous multithreading
    int systemProcessors = std::thread::hardware_concurrency(); // number of processors reported by operating system

    int abcd[4] = { 0,0,0,0 };                   // CPUID output
    cpuid(abcd, 0);                              // CPUID function 0

    int maxLeaf = abcd[0];                       // maximum eax input for CPUID
    if (abcd[2] == 0x6C65746E) {                 // last 4 chars of "GenuineIntel"
        vendor = 1;
    }
    else if (abcd[2] == 0x444D4163) {            // last 4 chars of "AuthenticAMD"
        vendor = 2;
    }
    else if (abcd[2] == 0x736C7561) {            // last 4 chars of "CentaurHauls"
        vendor = 3;
    }

    if (maxLeaf >= 1) {
        cpuid(abcd, 1);
        if (abcd[3] & (1 << 28)) {               // hyperthreading supported
            hyperthreadingSupported = true;
        }
    }

    if (vendor == 1) {
        //////////////////
        //    Intel     //
        //////////////////

        int hyper = 0;                           // hyperthreading status: 0 = unknown, 1 = disabled, 2 = enabled
        if (maxLeaf >= 0xB) {                    // leaf 0xB or 0x1F: Extended Topology Enumeration
            int num = 0xB;
            // if (maxLeaf >= 0x1F) num = 0x1F;

            for (int c = 0; c < 5; c++) {
                cpuid(abcd, num, c);             // enumeration level c
                int type = (abcd[2] >> 8) & 0xFF;// enumeration type at level c
                if (type == 1) {                 // SMT level
                    procPerCore = abcd[1] & 0xFFFF;
                }
                else if (type >= 2) {            // core level
                    logicalProc = abcd[1] & 0xFFFF;
                }
                else if (type == 0) break;
                // There are more types/levels to consider if we use num = 0x1F. We may need  
                // to fix this in the future if CPUs with more complex configurations appear
            }
            physicalProc = logicalProc / procPerCore;

            // The number of performance monitor registers depends on hyperthreading status
            // on Intel CPUs with performance monitoring version 3 or 4
            cpuid(abcd, 0xA, 0);                 // performance monitor counters information
            int perfVersion = abcd[0] & 0xFF;    // performance monitoring version
            int perfNum = (abcd[0] >> 8) & 0xFF; // number of performance monitoring registers
            if (perfVersion == 3 || perfVersion == 4) {
                if (perfNum == 4) {
                    hyper = 2;                   // 4 performance registers when hyperthreading enabled
                }
                else if (perfNum == 8) {         // 8 performance registers when hyperthreading disabled
                    hyper = 1;
                    procPerCore = 1;
                    logicalProc = physicalProc;  // reduce the number of logical processors when hyperthreading is disabled
                }
                // hyper remains 0 in all other cases, indicating unknown status
            }
        }
        else if (maxLeaf >= 4) {                 // CPUID function 4: cache parameters and cores
            cpuid(abcd, 4);
            logicalProc = (abcd[0] >> 26) + 1;
            if (hyperthreadingSupported) {
                // number of logical processors per core is not known. Assume 2 if hyperthreading supported
                procPerCore = 2;
            }
            physicalProc = logicalProc / procPerCore;
        }
        else {
            // no information. Assume 1 processor
        }
        if (systemProcessors > logicalProc) {
            // Multiple CPU chips. Assume that chips are identical with respect to hypethreading
            physicalProc = systemProcessors * physicalProc / logicalProc;
            logicalProc = systemProcessors;
        }
        else if (logicalProc > systemProcessors && systemProcessors > 0 && hyper == 0) {
            // Hyperthreading is disabled
            logicalProc = systemProcessors;
            physicalProc = systemProcessors;        
        }
    }
    else if (vendor == 2) {

        //////////////////
        //    AMD       //
        //////////////////

        cpuid(abcd, 0x80000000);                 // AMD specific CPUID functions
        int maxLeaf8 = abcd[0] & 0xFFFF;         // maximum eax 0x8000.... input for CPUID

        if (maxLeaf8 >= 8) {
            cpuid(abcd, 0x80000008);
            logicalProc = (abcd[2] & 0xFF) + 1;

            if (maxLeaf8 >= 0x1E) {
                cpuid(abcd, 0x8000001E);
                procPerCore = ((abcd[1] >> 8) & 0x03) + 1;
                // procPerCore = 2 if simultaneous multithreading is enabled, 1 if disabled
            }
            else {
                if (hyperthreadingSupported) {
                    procPerCore = 2;
                }
                else {
                    procPerCore = 1;
                }
            }
            physicalProc = logicalProc / procPerCore;
        }
        else if (hyperthreadingSupported) {
            // number of logical processors per core is not known. Assume 2 if SMT supported
            logicalProc = 2;
            physicalProc = 1;
        }
        if (systemProcessors > logicalProc) {
            // Multiple CPU chips. Assume that chips are identical with respect to SMT
            physicalProc = systemProcessors * physicalProc / logicalProc;
            logicalProc = systemProcessors;
        }
    }
    else {
    
        //////////////////////////////
        //    VIA or unknown CPU    //
        //////////////////////////////

        // The behavior of VIA processors is undocumented! It is not known how to detect threads on a VIA processor
        physicalProc = logicalProc = systemProcessors;
        if (hyperthreadingSupported && physicalProc > 1) {
            physicalProc /= 2;
        }
    }
    if (logical_processors) {
        // return logical_processors if pointer is not null
        *logical_processors = logicalProc;
    }
    return physicalProc;
}

/* Uncomment this for testing:

#include <stdio.h>

int main() {

    int logicalProc = 0;
    int physicalProc = physicalProcessors(&logicalProc); 

    printf("\nlogical processors: %i",  logicalProc);
    printf("\nphysical processors: %i", physicalProc);
    printf("\nlogical processors per core: %i", logicalProc / physicalProc);
    int sysproc = std::thread::hardware_concurrency();
    printf("\nsystem processors: %i", sysproc); 

    return 0;
}
*/