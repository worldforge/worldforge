/*
        CopyTest.cpp
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include "CopyTest.h"

int main(int argc, char** argv)
{
	Py_Initialize();

	CopyTest* app = new CopyTest();

	ADebug::setDebug(10);

	app->execute();

	return 0;	
}

#ifdef _WIN32
int WINAPI WinMain( 
	HINSTANCE hInstance, 	  // handle to current instance 
	HINSTANCE hPrevInstance,  // handle to previous instance 
	LPSTR lpCmdLine,	  // pointer to command line 
	int nCmdShow		  // show state of window 
) {
	main(0,NULL);
}
#endif
 
void CopyTest::execute()
{

        AObject test = AObject::mkMap();


	for (long i=0; i<10000; i++)
	        test = AObject::mkMap();
}
