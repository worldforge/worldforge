#ifndef TEST_ENTITY_H
#define TEST_ENTITY_H

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>


class TestEntity : public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE(TestEntity);
    CPPUNIT_TEST(testAccessors);
    CPPUNIT_TEST_SUITE_END();
    
public:
    TestEntity();

    void testAccessors();
    void testDefaults();

    virtual void setUp();
    virtual void tearDown();
protected:
};

#endif
