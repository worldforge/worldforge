/*
    Test module for atlas

    Tests functions of class Object
    by Gordon Hart, fex <who@which.net>

    29 Jan 2000

*/

#include "../Message/Object.h"
using Atlas::Message::Object;

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
bool mapTest();

//________________________________________________________________________________
//Main

int main() {

    println("Object test - ");

    int error = 0;

    error += !mapTest();


    println("Test run finished");

    if ( verbose )
      cout << testCount << " tests performed.\n";

    if ( error )
        return -1;

    return 0;

}

//________________________________________________________________________________
//subchecks

void mapTypeCheck( const Object& anObject) {

    check(  anObject.isMap() );
    check( !anObject.isList() );
    check( !anObject.isInt() );
    check( !anObject.isFloat() );
    check( !anObject.isString() );
}

//________________________________________________________________________________
//Main checks

bool mapTest() {

    try {

        println( "Creating map" );

        Object map;

        println( "Type check 1" );
        mapTypeCheck( map );

        println( "Check has()" );
        check( !map.has( "" ) );
        check( !map.has( "map") );

        println( "Check length" );
        check ( map.length() == 0 );

        println( "Adding value" );
        map.set( "string", "Stringval first" );

        println( "Consistency checks" );
        check( map.length() == 1 );
        check( map.has( "string" ) );
        check( !map.has( "" ) );

        println( "Overwriting value" );
        map.set( "string", "Stringval second" );

        check( map.length() == 1 );
        check( map.has( "string" ) );
        check( !map.has( "" ) );
        check( !map.has( "Stringval second" ) );

        string test;
        check( map.get( "string", test ) );
        check( map.has( "string" ) );
        check( test == "Stringval second" );

        println( "Delete check 1" );
        map.del( "" );
        check( map.has( "string" ) );
        check( map.length() == 1 );
        map.del( "string" );
        check( !map.has( "string" ) );
        check( map.length() == 0 );


        println( "Delete check 2" );
        map.set( "1", "2" );
        map.set( "3", "4" );
        check( map.length() == 2 );
        check( map.has("1") );
        check( map.has("3") );
        check( !map.has("2" ) );
        map.del( "1" );
        check( map.length() == 1 );
        check( !map.has( "1" ) );
        map.del( "3" );
        check( map.length() == 0 );
        check( !map.has( "3" ) );

        println( "Clearing map" );
        map.set( "1", "2" );
        map.set( "3", "4" );
        map.clear();
        check( map.length() == 0 );

        println( "Integer add" );
        map.set( "int", 9191 );
        check( map.has( "int" ) );
        check( map.length() == 1 );

        println( "double add" );
        map.set( "double", 2828.2828 );
        check( map.has( "double" ) );
        check( map.length() == 2 );

        println( "string add" );
        map.set( "string", "a test String" );
        check( map.has( "string" ) );
        check( map.length() == 3 );

        println( "long add" );
        map.set( "long", 12345678L );
        check( map.has( "long" ) );
        check( map.length() == 4 );

        Object foo;
        println( "Convert to value List check" );
        foo = map.vals();
        check( foo.length() == 4 );

        println( "Convert to key List check" );
        foo = map.vals();
        check( foo.length() == 4 );


        int ival=       0;
        double dval=    0;
        long lval=      0;
        string sval=    "";

        println( "Integer true get" );
        check( map.get( "int", ival ) );
        check( ival == 9191 );
        println( "Integer get as Long" );
        check( map.get( "int", lval ) );
        check( ival == 9191L );
        println( "Bad type get" );
        check( !map.get( "int", dval ) );
        check( !map.get( "int", sval ) );

        dval = lval = ival = 0;
        sval = "";
        println( "Double true get" );
        check( map.get( "double", dval ) );
        check( dval != 0 );
        println( "Bad type get" );
        check( !map.get( "double", ival ) );
        check( !map.get( "double", sval ) );
        check( !map.get( "double", lval ) );

        dval = lval = ival = 0;
        sval = "";
        println( "Long true get" );
        check( map.get( "long", lval ) );
        check( lval == 12345678L );
        println( "Long get as integer" );
        check( map.get( "long", ival ) );
        check( ival == 12345678L );
        println( "Bad type get" );
        check( !map.get( "long", dval ) );
        check( !map.get( "long", sval ) );

        dval = lval = ival = 0;
        sval = "";
        println( "String true get" );
        check( map.get( "string", sval ) );
        check( sval == "a test String" );
        println( "Bad type get" );
        check( !map.get( "string", ival ) );
        check( !map.get( "string", dval ) );
        check( !map.get( "string", lval ) );

        println( "Embedded object test" );
        Object bar;
        Object bar2;
        bar.set( "object", map );
        check( bar.length() == 1);
        check( map.length() == 4);
        check( bar.has( "object" ) );
        check( bar.get( "object", bar2 ) );
        check( bar2.length() == 4 );

        println( "Key list check" );
        Object foobar = bar2.keys();
        check( foobar.length() == 4 );


        
        println( "Out of bounds ndx check on key list" );
        for ( int i = -10 ; i < 10; ++i) {
            if ( ( i < 0) || ( i > 3 ) ) {
                check( !foobar.get( i, sval ) );
            }
            else {
                check( foobar.get( i, sval ) );
            }
        }
        
        /*
        // out of bound calls on list are broken,this shows it up
        // the code above is the proper check for correct behaviour
        println( "Out of bounds ndx check on key list" );
        for ( int i = -10 ; i < 10; ++i) {
            sval = "";
            cout << foobar.get( i, sval);
            cout << "At map[" << i << "] is the string [" << sval << "]" << endl;
        }
        */




        println( "Closing clear checks" );
        bar.clear();
        check( bar.length() == 0 );
        check( map.length() == 4 );
        map.clear();
        check( map.length() == 0 );


        return true;

    } catch ( TestError& e ) {

        print("...Failed\n");
        return false;
    }

}
