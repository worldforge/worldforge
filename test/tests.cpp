
#include "testConnection.h"
#include "testPlayer.h"

int main(int argc, char **argv)
{
	
	TestConnection tc;
	
	tc.setUp();
	tc.testConnect();
	tc.tearDown();
	
	
	TestPlayer tp;
	
	tp.setUp();
	tp.testLogin();
	tp.tearDown();
	
	tp.setUp();
	tp.testAccountCreate();
	tp.tearDown();
	
	/*    
	CppUnit::TextTestRunner runner;

    CppUnit::Test* tp =
        CppUnit::TestFactoryRegistry::getRegistry().makeTest();

    runner.addTest(tp);
    bool passed = runner.run();

    if (!passed) {
        return 1;
    }
  */  
    return 0; // tests passed okay
}


