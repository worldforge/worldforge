
#include <cppunit/TextTestRunner.h>

#include "testPlayer.h"
#include "testConnection.h"
#include "testLobby.h"

CPPUNIT_TEST_SUITE_REGISTRATION(TestConnection);
CPPUNIT_TEST_SUITE_REGISTRATION(TestPlayer);
CPPUNIT_TEST_SUITE_REGISTRATION(TestLobby);

int main(int argc, char **argv)
{
    CppUnit::TextTestRunner runner;

    CppUnit::Test* tp =
        CppUnit::TestFactoryRegistry::getRegistry().makeTest();

    runner.addTest(tp);
    bool passed = runner.run();

    if (!passed) {
        return 1;
    }
    
    return 0;
}


