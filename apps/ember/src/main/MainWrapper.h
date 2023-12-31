/**
 * @file This header is defining startup symbols and calls the main automatically.
 */

#ifndef MAINWRAPPER_H
#define MAINWRAPPER_H

extern "C"
int main(int argc, char* argv[]);

#ifdef _MSC_VER
//include these for __argc and __argv variables.
#include <windows.h>
#include <stdlib.h>

/**
 * @brief In win32 application, where the console window is hidden, the WinMain symbol is called instead of main symbol.
 */
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
	main(__argc, __argv);
}
#endif

#endif