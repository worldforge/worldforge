/*
    Test module for atlas

    Tests functions of class Object
    by Gordon Hart, fex <who@which.net>

    29 Jan 2000

    test-object-list runs test on list type atlas objects, as defined in
    the class Atlas::Object in file src/Object/Object.h

*/

#include "../Object/Object.h"
using Atlas::Object;

#include <iostream>
using std::cout;
using std::endl;

//________________________________________________________________________________
//Global
bool verbose = true;
int  testCount = 0;
//________________________________________________________________________________
//Helper inlines

inline void print ( const string& output ) {

    if (verbose) {
        cout << output;
        cout.flush();
    }
}

inline void println ( const string& output ) {

    if (verbose) {
        cout << endl << output << endl;
        cout.flush();
    }
}


class TestError {};

inline void check( const bool test ) {
    ++testCount;
    if (!test)
        throw TestError();
    print(".Ok");
}

//________________________________________________________________________________
//Proto
bool listTest();

//________________________________________________________________________________
//Main

int main() {

    println("List Object Test - ");

    int error = 0;

    error += !listTest();

    println("Test run finished");

    if ( verbose )
      cout << testCount << " tests performed.\n";

    if ( error )
        return -1;

    return 0;

}

//________________________________________________________________________________
//subchecks

void listTypeCheck( const Object& anObject) {

    check( !anObject.isMap() );
    check( anObject.isList() );
    check( !anObject.isInt() );
    check( !anObject.isFloat() );
    check( !anObject.isString() );
}

//________________________________________________________________________________
//Main checks

bool listTest() {

    try {

      return true;

    } catch ( TestError& e ) {

        print("...Failed\n");
        return false;
    }

}
