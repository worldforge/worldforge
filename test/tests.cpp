
#include <cppunit/TextTestRunner.h>

#include "testPlayer.h"
#include "testConnection.h"

CPPUNIT_TEST_SUITE_REGISTRATION(TestConnection);
CPPUNIT_TEST_SUITE_REGISTRATION(TestPlayer);

int main(int argc, char **argv)
{
    CppUnit::TextTestRunner runner;

    CppUnit::Test* tp =
        CppUnit::TestFactoryRegistry::getRegistry().makeTest();

    runner.addTest(tp);
    runner.run();
    
    return 0;
}


