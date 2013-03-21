
#ifdef _WIN32

        #include <windows.h>

#else

        #include <sys/mman.h>

        #include <unistd.h>

        #include <string.h>

        #include <stdlib.h>

        //typedef unsigned char byte;

        #define _stricmp strcasecmp

#endif

 

 

// ==================================================

// UnlockMemory (WIN32 & Linux compatible)

// --------------------------------------------------

// Makes the memory at address writable for at least

// size bytes.

// Returns 1 if successful, returns 0 on failure.

// ==================================================

static int UnlockMemory(int address, int size) {

        int ret;

        int dummy;

#ifdef _WIN32

        ret = VirtualProtect((LPVOID)address, size, PAGE_EXECUTE_READWRITE, &dummy);

        return (ret != 0);

#else

        // Linux is a bit more tricky

        int page1, page2;

        page1 = address & ~( getpagesize() - 1);

        page2 = (address+size) & ~( getpagesize() - 1);

        if( page1 == page2 ) {

                ret = mprotect((char *)page1, getpagesize(), PROT_READ | PROT_WRITE | PROT_EXEC);

        } else {

                ret = mprotect((char *)page1, getpagesize(), PROT_READ | PROT_WRITE | PROT_EXEC);

                if (ret) return 0;

                ret = mprotect((char *)page2, getpagesize(), PROT_READ | PROT_WRITE | PROT_EXEC);

                return (ret == 0);

        }

#endif

}

 

// ==================================================

// LockMemory (WIN32 & Linux compatible)

// --------------------------------------------------

// Makes the memory at address read-only for at least

// size bytes.

// Returns 1 if successful, returns 0 on failure.

// ==================================================

static int LockMemory(int address, int size) {

        int ret;

#ifdef _WIN32

        ret = VirtualProtect((LPVOID)address, size, PAGE_EXECUTE_READ, NULL);

        return (ret != 0);

#else

        // Linux is a bit more tricky

        int page1, page2;

        page1 = address & ~( getpagesize() - 1);

        page2 = (address+size) & ~( getpagesize() - 1);

        if( page1 == page2 ) {

                ret = mprotect((char *)page1, getpagesize(), PROT_READ | PROT_EXEC);

        } else {

                ret = mprotect((char *)page1, getpagesize(), PROT_READ | PROT_EXEC);

                if (ret) return 0;

                ret = mprotect((char *)page2, getpagesize(), PROT_READ | PROT_EXEC);

                return (ret == 0);

        }

#endif

}

 

#ifdef _WIN32

#define _CLIENTCAPPOS1  0x4427F6

#define _CLIENTCAPPOS2  0x4426B6

#else

#define _CLIENTCAPPOS1  0x8054D91

#endif

 

 

/* Call the following when the server starts up */

void openMemLocks(void){
        UnlockMemory(_CLIENTCAPPOS1,1);

        *(unsigned char *)_CLIENTCAPPOS1 = (unsigned char)0x40;

        LockMemory(_CLIENTCAPPOS1,1);

#ifdef _CLIENTCAPPOS2

        UnlockMemory(_CLIENTCAPPOS2,1);

        *(unsigned char *)_CLIENTCAPPOS2 = (unsigned char)0x40;

        LockMemory(_CLIENTCAPPOS2,1);

#endif
}