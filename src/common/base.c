#include "bofdefs.h"
//#include "beacon.h"
#include "time.h"
#include "badger_exports.h"
#ifndef bufsize
#define bufsize 8192
#endif


WCHAR **g_dispatch = NULL;

char * output __attribute__((section (".data"))) = 0;  // this is just done so its we don't go into .bss which isn't handled properly
WORD currentoutsize __attribute__((section (".data"))) = 0;
HANDLE trash __attribute__((section (".data"))) = NULL; // Needed for x64 to not give relocation error

#ifdef BOF
int bofstart();
void internal_printf(const char *fmt, ...);
void printoutput(BOOL done);
#endif
char * Utf16ToUtf8(const wchar_t* input);
#ifdef BOF
int bofstart()
{   
    output = (char*)MSVCRT$calloc(bufsize, 1);
    currentoutsize = 0;
    return 1;
}

/*
void internal_printf(const char *fmt, ...) {
    va_list args;
    char *label, *value;

    va_start(args, fmt);
    label = va_arg(args, char*);
    value = va_arg(args, char*);
    va_end(args);

    BadgerDispatch(g_dispatch, "%s %s\n", label, value);
}*/

void internal_printf(const char *fmt, ...) {
    char buf[1024];
    char *p = buf;
    char *end = buf + sizeof(buf) - 1;
    va_list args;
    va_start(args, fmt);

    while (*fmt && p < end) {
        if (*fmt == '%') {
            fmt++;
            // skip flags/width/precision (z.B. -30)
            while (*fmt && *fmt != 's' && *fmt != '%') fmt++;
            if (*fmt == 's') {
                char *s = va_arg(args, char*);
                while (s && *s && p < end) *p++ = *s++;
                fmt++;
            } else if (*fmt == '%') {
                *p++ = '%';
                fmt++;
            }
        } else {
            *p++ = *fmt++;
        }
    }
    *p = '\0';
    va_end(args);

    BadgerDispatch(g_dispatch, "%s", buf);
}

void printoutput(BOOL done)
{

    char * msg = NULL;
    //BeaconOutput(CALLBACK_OUTPUT, output, currentoutsize);
    //BadgerDispatch(dispatch, "%s\n", "test");
    currentoutsize = 0;
    memset(output, 0, bufsize);
    if(done) {MSVCRT$free(output); output=NULL;}
}
#else
#define internal_printf printf
#define printoutput 
#define bofstart 
#endif

// Changes to address issue #65.
// We can't use more dynamic resolve functions in this file, which means a call to HeapRealloc is unacceptable.
// To that end if you're going to use this function, declare how many libraries you'll be loading out of, multiple functions out of 1 library count as one
// Normallize your library name to uppercase, yes I could do it, yes I'm also lazy and putting that on the developer.
// Finally I'm going to assume actual string constants are passed in, which is to say don't pass in something to this you plan to free yourself
// If you must then free it after bofstop is called
#ifdef DYNAMIC_LIB_COUNT


typedef struct loadedLibrary {
    HMODULE hMod; // mod handle
    const char * name; // name normalized to uppercase
}loadedLibrary, *ploadedLibrary;
loadedLibrary loadedLibraries[DYNAMIC_LIB_COUNT] __attribute__((section (".data"))) = {0};
DWORD loadedLibrariesCount __attribute__((section (".data"))) = 0;

BOOL intstrcmp(LPCSTR szLibrary, LPCSTR sztarget)
{
    BOOL bmatch = FALSE;
    DWORD pos = 0;
    while(szLibrary[pos] && sztarget[pos])
    {
        if(szLibrary[pos] != sztarget[pos])
        {
            goto end;
        }
        pos++;
    }
    if(szLibrary[pos] | sztarget[pos]) // if either of these down't equal null then they can't match
        {goto end;}
    bmatch = TRUE;

    end:
    return bmatch;
}

//GetProcAddress, LoadLibraryA, GetModuleHandle, and FreeLibrary are gimmie functions
//
// DynamicLoad
// Retrieves a function pointer given the BOF library-function name
// szLibrary           - The library containing the function you want to load
// szFunction          - The Function that you want to load
// Returns a FARPROC function pointer if successful, or NULL if lookup fails
//
FARPROC DynamicLoad(const char * szLibrary, const char * szFunction)
{
    FARPROC fp = NULL;
    HMODULE hMod = NULL;
    DWORD i = 0;
    DWORD liblen = 0;
    for(i = 0; i < loadedLibrariesCount; i++)
    {
        if(intstrcmp(szLibrary, loadedLibraries[i].name))
        {
            hMod = loadedLibraries[i].hMod;
        }
    }
    if(!hMod)
    {
        hMod = LoadLibraryA(szLibrary);
        if(!hMod){ 
            //BeaconPrintf(CALLBACK_ERROR, "*** DynamicLoad(%s) FAILED!\nCould not find library to load.", szLibrary);
            return NULL;
        }
        loadedLibraries[loadedLibrariesCount].hMod = hMod;
        loadedLibraries[loadedLibrariesCount].name = szLibrary; //And this is why this HAS to be a constant or not freed before bofstop
        loadedLibrariesCount++;
    }
    fp = GetProcAddress(hMod, szFunction);

    if (NULL == fp)
    {
        //BeaconPrintf(CALLBACK_ERROR, "*** DynamicLoad(%s) FAILED!\n", szFunction);
    }
    return fp;
}
#endif


char* Utf16ToUtf8(const wchar_t* input)
{
    int ret = Kernel32$WideCharToMultiByte(
        CP_UTF8,
        0,
        input,
        -1,
        NULL,
        0,
        NULL,
        NULL
    );

    char* newString = (char*)intAlloc(sizeof(char) * ret);

    ret = Kernel32$WideCharToMultiByte(
        CP_UTF8,
        0,
        input,
        -1,
        newString,
        sizeof(char) * ret,
        NULL,
        NULL
    );

    if (0 == ret)
    {
        goto fail;
    }

retloc:
    return newString;
/*location to free everything centrally*/
fail:
    if (newString){
        intFree(newString);
        newString = NULL;
    };
    goto retloc;
}

//release any global functions here
void bofstop()
{
#ifdef DYNAMIC_LIB_COUNT
    DWORD i;
    for(i = 0; i < loadedLibrariesCount; i++)
    {
        FreeLibrary(loadedLibraries[i].hMod);
    }
#endif
	return;
}

void InitRandomSeed()
{
    MSVCRT$srand((unsigned int)MSVCRT$time(NULL));
}

//
// Helper func to generate random strings for prodecure names, etc
//
char* GenerateRandomString(int length)
{
    char* result = (char*)intAlloc(length + 1);
    for (int i = 0; i < length; i++)
    {
        result[i] = (char)(MSVCRT$rand() % 26 + 97);
    }
    result[length] = '\0';
    return result;
}
