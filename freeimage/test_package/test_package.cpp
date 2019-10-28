#include <FreeImage.h>

#include <string>
#include <iostream>
#include <stdio.h>

using namespace std;

#define NEED_INIT defined(FREEIMAGE_LIB) || !defined(WIN32)

void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message) {
    printf("FreeImage error: '%s'\n", message);
}

int testLib(int argc, const char** argv)
{
#if NEED_INIT
    FreeImage_Initialise();
#endif

    FreeImage_SetOutputMessage(FreeImageErrorHandler);
    
    const char * image_file = argc > 1 ? argv[1] : "./test.png";
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

	// check the file signature and get its format
	// (the second argument is currently not used by FreeImage)
	fif = FreeImage_GetFileType(image_file, 0);
	if(fif == FIF_UNKNOWN) {
		// no signature ?
		// try to guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilename(image_file);
	}
	// check that the plugin has reading capabilities ...
	if((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif)) {
		// Load the file
		FIBITMAP* dib = FreeImage_Load(fif, image_file, 0);
		if(!dib)
			return 1;
		FreeImage_Unload(dib);
		return 0;
	}
	return 1;
#if NEED_INIT
    FreeImage_DeInitialise(); 
#endif   
}

int main(int argc, const char ** argv){
    cout << "************* Testing freeimage lib **************" << endl;

    int ret = testLib(argc, argv);
    
    cout << "**************************************************" << endl;

    return ret;
}
