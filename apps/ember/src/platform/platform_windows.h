//This file contains windows specific code
#ifndef PLATFORM_WINDOWS_H
#define PLATFORM_WINDOWS_H

#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <direct.h>
#include <stdio.h> //_snprintf
#include <string.h> //_stricmp

#define putenv _putenv
#define unlink _unlink
#define chdir _chdir
#define stricmp _stricmp
#define access _access

#endif //PLATFORM_WINDOWS_H

