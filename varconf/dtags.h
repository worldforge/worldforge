///////////////////////////////////////////////////////////////////////////
//                                  dtags.h
//
// Copyright (c) 1996 Bryce W. Harrington
// - bryce@netcom.com - bryce@alumni.caltech.edu - bryce@python.sp.trw.com
// = http://www.alumni.caltech.edu/~bryce = http://www.genealogy.org/~bryce
//
//-----------------------------------------------------------------------
// License:  This code may be used by anyone for any purpose
//           so long as the copyright notices and this license
//           statement remains attached.
//-----------------------------------------------------------------------
// Description of file
//  This is a simple debug message system.
//
//----------------------------------------------------------------------//
// MODIFICATION HISTORY
//
// June 1993 - Bryce W. Harrington
//   Created DTAGS facility as part of the basicdefs.h include file
//
// October 1996 - Bryce W. Harrington
//   Broke out DTAGS into its own file
//
// July 1999
//   added console fluff for qserver2 (sdt@gmx.net)
//
// Sept 1999
//    added log to file option, see USE_LOG_FILE define (fex who@which.net)
//    now inlines rather than macros

// Sept 1999 - WF
//  sorry, I`ve been pissing about with this file and it bears no relation to Bryce`s
//  tidy original version, blame me ;) - fex

//////////////////////////////////////////////////////////////////////////
#define DEBUG_TAG
#ifndef dtags_h
    #define dtags_h

    // Define for console operations
//    #define CONSOLE


    #include <iostream>
    using std::cout;
    using std::cerr;
    using std::endl;
    using std::ios;
    //////////////////////////////////////////////////////////////////////////

    #ifdef CONSOLE
        #include "Console.h"
        extern Console* console;
    #endif

    ///////////////////////////////////////////////////////////////////////
    // Debug Message Handling

    //optional mirror output to logfile
    #define USE_LOG_FILE
    #ifdef  USE_LOG_FILE
        #include <fstream>
        using std::ofstream;
    #endif



    #ifdef DEBUG_TAG

    inline void send(const string& msg, const string& type) {
    #ifdef CONSOLE
            console->PostMsg(Console::GENERAL, type + msg);
    #else
            cerr << "!" << msg << endl;
    #endif
    #ifdef USE_LOG_FILE
            ofstream deb("dtags.log",ios::app);
            deb << "!" << msg << endl;
            deb.close();
    #endif
    }

    inline void INFO(const string& msg) {
        send(msg, "INFO: ");
    }

    inline void STAT(const string& msg) {
        send(msg, "STAT: ");
    }

    inline void DTAG(const string& msg) {
        send(msg, "DTAG: ");
    }

    inline void WARN(const string& msg) {
        send(msg, "WARN: ");
    }


        #ifdef CONSOLE
        #define D2TAG( msg, a )		console->PostMsg(Console::GENERAL, \
        				(string)"DBUG: "+ (string)msg +    \
        				(string)" "+ (string)a)



        #define D3TAG( msg, a, b )	console->PostMsg(Console::GENERAL,     \
        				(string)"DBUG: "+ (string)msg +        \
        				(string)" "+ (string)a + (string)" " + \
        				(string)b)


        #define FATAL( msg )            console->PostMsg(Console::GENERAL,      \
        				(string)"OUCH: ["+ (string)__FILE__ +   \
                                        (string)" Fatal Error]: " + (string)msg)



        #define ASSERT( test, msg )    if (!(test)) { \
                                        console->PostMsg(Console::GENERAL,          \
        				(string)"OUCH: ["+ (string)__FILE__ +       \
                                        (string)" Failed Assert]: "+ (string)msg); }

        // ---------------------------------------------------------------- //
        // Standard Out
        #else
            #define D2TAG( msg, a )		cerr << msg << " " << a << endl
            #define D3TAG( msg, a, b )	cerr << msg << " " << a << " " << b << endl
            #define FATAL( msg )            cerr << __FILE__ << ":" << __LINE__ \
                                                 << "  Fatal Error: " << msg << endl
            #define ASSERT( test, msg )    if (!(test)) { \
                                               cerr << __FILE__ << ":" << __LINE__ \
                                                    << "  Failed Assert: " << msg << \
            				     endl; assert(test); }
        #endif
    #else
        #define DTAG( msg )
        #define D2TAG( msg, a )
        #define D3TAG( msg, a, b )
        #define FATAL( msg )
        #define ASSERT( test, msg )
    #endif
#endif // dtags.h
