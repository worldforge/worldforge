/////////////////////////////////////////////////////////////////////////
//                               ftos.cpp
//
// Copyright (c) 1996 Bryce W. Harrington
// - bryce@netcom.com - bryce@alumni.caltech.edu - bryce@python.sp.trw.com
//                 http://www.alumni.caltech.edu/~bryce
//
//-----------------------------------------------------------------------
// License:  This code may be used by anyone for any purpose
//           so long as the copyright notices and this license
//           statement remains attached.
//-----------------------------------------------------------------------
//
// string ftos(double val[, char mode[, int sigfig[, int precision[, int options]]]])
//
//  DESCRIPTION
//    This routine is intended to replace the typical use of sprintf for
//    converting floating point numbers into strings.  The motivation for
//    this effort is that in C++ the stdio library is rarely used; in 
//    engineering applications using GUI interfaces the only use for
//    stdio is for converting doubles to strings.  By using this routine
//    sprintf will not be needed except for special circumstances.
//
//    To one-up sprintf, an additional mode was created - 'h' mode - 
//    which produces numbers in 'engineering notation' - exponents are
//    always shown in multiples of 3.  To non-engineers this mode is
//    probably irrelevant, but for engineers (and scientists) it is SOP.
//    
//    One other new feature is an option to use 'x10^' instead of the
//    conventional 'E' for exponental notation.  This is entirely for
//    aesthetics since numbers in the 'x10^' form cannot be used as
//    inputs for most programs.
//
//    For most cases, the routine can simply be used with the defaults
//    and acceptable results will be produced.  No fill zeros or trailing
//    zeros are shown, and exponential notation is only used for numbers
//    greater than 1e6 or less than 1e-3.
//
//    The one area where sprintf may surpass this routine is in width control.
//    No provisions are made in this routine to restrict a number to a
//    certain number of digits (thus allowing the number to be constrained
//    to an 8 space column, for instance.)  Along with this, it does not
//    support pre-padding a number with zeros (e.g., '5' -> '0005') and will
//    not post-pad a number with spaces (i.e., allow left-justification.)
//
//    If width control is this important, then the user will probably want to
//    use the stdio routines, which really is well suited for outputting 
//    columns of data with a brief amount of code.
//
//  PARAMETERS
//    val        - number to be converted
//    mode       - can be one of four possible values.  Default is 'g'
//
//                 e: Produces numbers in scientific notation.  One digit
//                    is shown on the left side of the decimal, the rest 
//                    on the right, and the exponential is always shown.
//                    EXAMPLE:  1.04e-4
//
//                 f: Produces numbers with fixed format.  Number is shown
//                    exact, with no exponent.  
//                    EXAMPLE:  0.000104
//
//                 g: If val is greater than 1e6 or less than 1e-3 it will
//                    be shown in 'e' format, otherwise 'f' format will be
//                    used.
//
//                 h: Produces numbers in engineering format.  Result is
//                    identical to 'f' format for numbers between 1 and
//                    1e3, otherwise, the number is shown such that it 
//                    always begins with a nonzero digit on the left side
//                    (unless number equals zero), and the exponential is
//                    a multiple of 3.
//                    EXAMPLE:  104e-6
//
//                 If the mode is expressed as a capital letter (e.g., 'F')
//                 then the exponential part of the number will also be 
//                 capitalized (e.g., '1E6' or '1X10^6'.)
//
//    sigfig     - the number of significant figures.  These are the digits
//                 that are "retained".  For example, the following numbers
//                 all have four sigfigs:
//                    1234       12.34      0.0001234       1.234e-10
//                 the last digit shown will be rounded in the standard
//                 manner (down if the next digit is less than 5, up otherwise.)
//
//    precision  - the number of digits to show to the right of the decimal.
//                 For example, all of the following numbers have precisions
//                 of 2:
//                    1234.00     12.34     0.00     1.23e-10   123.40e-12
//
//    options    - several options are allowed to control the look of the
//                 output.
//
//               FORCE_DECIMAL - require the decimal point to be shown for
//                 numbers that do not have any fractional digits (or that 
//                 have a precision set to zero)
//                 EXAMPLE:  1.e6
//               FORCE_EXP_ZERO - pad the 10's zero in exponent if necessary
//                 EXAMPLE:  1e06
//               FORCE_HUNDRED_EXP_ZERO - pad the 100's zero in exponent if
//                 necessary.  Also pads 10's zero in exponent if necessary.
//                 EXAMPLE:  1e006
//               FORCE_EXP_PLUS - show the '+' in the exponent if exponent 
//                 is used.
//                 EXAMPLE:  1e+6
//               FORCE_EXP - force the output to display the exponent
//                 EXAMPLE:  0e0
//               FORCE_X10 - use x10^ instead of E
//                 EXAMPLE:  1x10^6
//               FORCE_PLUS - force output of the '+' for positive numbers
//                 EXAMPLE:  +1e6
//
//                 Options can be combined using the usual OR method.  For
//                 example,
//
//                 ftos(123.456, 'f', -1, -1, FORCE_PLUS | FORCE_X10 | FORCE_EXP)
//
//                 gives "+123.456x10^0"
//
//  RETURN VALUE
//    The string representation of the number is returned from the routine.
//    The ANSI C++ Standard "string" class was used for several important
//    reasons.  First, because the string class manages it's own space, the
//    ftos routine does not need to concern itself with writing to unallocated
//    areas of memory or with handling memory reallocation internally.  Second,
//    it allows return of an object, not a pointer to an object; this may not
//    be as efficient, but it is cleaner and safer than the alternative.  Third,
//    the routine's return value can be directly assigned to a variable, i.e.
//        string var = ftos(3.1415); 
//    which makes code much easier to comprehend and modify.
//
//    Internally, the ftos routine uses fairly typical string operators (=, +=,
//    +, etc.) which pretty much any other flavor of string class will define as
//    well.  Thus if one does not have access to the ANSI C++ Standard string
//    class, the user can substitute another with little difficulty.  (If the
//    alternate class is not named "string" then redefine "string" to whatever
//    you wish to use.  For example,
//        #define string CString
//    
// November 1996 - Bryce Harrington
//    Created ftoa and ftos
//
// December 1996 - Bryce Harrington
//    Added engineering notation mode, added sigfig capability, added
//    significant debug code, added options, thoroughly debugged and
//    tested the code.
//
//
// June 1999 - Bryce Harrington
//    Modified to run on Linux for WorldForge
//
/////////////////////////////////////////////////////////////////////////

#include <string>
#ifdef DEBUG
#undef DEBUG
#endif

// Define this so you can compile this file stand alone for testing
// Undefine it if you wish to include it in your code
#undef TESTFTOS

// This is the routine used for converting a floating point into a string
// This may be included in stdlib.h on some systems and may conflict.
// Let me know your system & etc. so I can properly #ifdef this, but
// try commenting the following four lines out if you run into conflicts.
// extern "C" {
// char*
// ecvt (double val, size_t ndigit, int *decpt, int *sign);
// }

#include "ftos.h"
#include "dtags.h"

#include <iostream.h>
// #include <conio.h>  // I think this is only needed on windows...

#undef DEBUG

// This routine counts from the end of a string like '10229000' to find the index
// of the first non-'0' character (5 would be returned for the above number.)
inline int
countDigs(char *p)
{
    int length =0;
    while (*(p+length)!='\0') length++;               // Count total length
    while (length>0 && *(p+length-1)=='0') length--;  // Scan backwards for a non-'0'
    return length;
}

// This routine determines how many digits make up the left hand
// side of the number if the abs value of the number is greater than 1, or the
// digits that make up the right hand side if the abs value of the number
// is between 0 and 1.  Returns 1 if v==0.  Return value is positive for numbers
// greater than or equal to 1, negative for numbers less than 0.1, and zero for
// numbers between 0.1 and 1.
inline int countLhsDigits(double v)
{
    if (v<0) v = -v;                   // Take abs value
    else if (v==0) return 1;           // Special case if v==0

    int n=0;
    for (; v<0.1; v*=10)               // Count digits on right hand side (l.t. 0.1)
        { n--; }
    for (; v>=1; v/=10)                // Count digits on left hand side (g.e. 1.0)
        { n++; }
    return n;
}

// This is the routine that does the work of converting the number into a string.
string ftos(double val, char mode, int sigfig, int precision, int options)
{
    #ifdef DEBUG
#ifdef CONSOLE
#else
        cerr << "\n*** Starting ftos(val=" << val << ", mode=" << mode 
             << ", sigfig=" << sigfig << ", prec=" << precision
             << ", options=" << options << ")" << endl;
#endif
    #endif

    // Parse the options to a more usable form
    // These options allow the user to control some of the ornaments on the
    // number that is output.  By default they are all false.  Turning them
    // on helps to "fix" the format of the number so it lines up in columns
    // better.  
    // - require the decimal point to be shown for numbers that do not have
    //   any fractional digits (or that have a precision set to zero
    bool forceDecimal = (options & FORCE_DECIMAL);
    // - show the 10's and 100's zero in exponent
    bool forceExpZero = (options & FORCE_EXP_ZERO);
    bool forceHundredExpZero = (options & FORCE_HUNDRED_EXP_ZERO);
    // - show the '+' in the exponent if exponent is used
    bool forceExpPlus = (options & FORCE_EXP_PLUS);
    // - force the output to display the exponent
    bool forceExponent = (options & FORCE_EXPONENT);
    // - use x10^ instead of E
    bool forcex10 = (options & FORCE_X10);
    // - force output of the '+' for positive numbers
    bool forcePlus = (options & FORCE_PLUS);

    #ifdef DEBUG
#ifdef CONSOLE
#else
        cerr << "Options:  "
             << "x10=" << (forcex10            ? "on" : "off" ) << " "
             << ".="   << (forceDecimal        ? "on" : "off" ) << " " 
             << "e0="  << (forceExpZero        ? "on" : "off" ) << " " 
             << "e00=" << (forceHundredExpZero ? "on" : "off" ) << " " 
             << "e+="  << (forceExpPlus        ? "on" : "off" ) << " " 
             << "e="   << (forceExponent       ? "on" : "off" ) << " "
             << "+#="  << (forcePlus           ? "on" : "off" ) << endl; 
#endif
    #endif

    // - exponent usage
    bool useExponent = false;

    // Determine the case for the 'e' (if used)
    char E = (forcex10)? 'x' : 'e';
    if (isupper(mode)) {
        E = toupper(E);
        mode = tolower(mode);
    }

    D2TAG("*** Mode is now", itos(mode));


    // Determine how many decimals we're interested in
    int L = countLhsDigits(val);
    D2TAG("*** L is", itos(L));
    int count = 0;
    if (sigfig==0)                     // bad input - don't want any sigfigs??!!
        return "";
    else if (precision>=0) {           // Use fixed number of decimal places
        count = precision;
        if (mode == 'e') count += 1;
        else if (mode == 'f') count += L;
        else if (mode == 'g') count += (L>6 || L<-3)? 1 : L;
        else if (mode == 'h') count += (L>0)? ((L-1)%3+1) : (L%3+3);
        if (sigfig>0) count = (sigfig > count)? count : sigfig;  // Use sigfig # if it means more decimal places
    }
    else if (sigfig>0)                 // Just use sigfigs
        count = sigfig;
    else                               // prec < 0 and sigfig < 0
        count = 10;
    D2TAG("*** count is", itos(count));

    // Get number's string rep, sign, and exponent
    int sign = 0;
    int decimal=0;
    register char *p = ecvt(val, count, &decimal, &sign);

    D2TAG("*** string rep is", p);
    D2TAG("*** decimal is", itos(decimal));
    D2TAG("*** sign is", itos(sign));

    // Count the number of relevant digits in the resultant number
    int dig = countDigs(p);
    if (dig < sigfig) dig = sigfig;
    D2TAG("*** digs is", itos(dig));

    // Determine number of digits to put on left side of the decimal point
    int lhs=0;
    // For 'g' mode, decide whether to use 'e' or 'f' format.
    if (mode=='g') mode = (decimal>6 || decimal<-3)? 'e' : 'f';
    switch (mode) {
        case 'e':
            lhs = 1;                   // only need one char on left side
            useExponent = true;        // force exponent use
            break;

        case 'f':
            lhs = (decimal<1)? 1 : decimal;
                                       // use one char on left for num < 1, 
                                       // otherwise, use the number of decimal places.
            useExponent = false;       // don't want exponent for 'f' format
            break;

        case 'h':
            if (val==0.0)              // special case for if value is zero exactly.
                lhs = 0;               // this prevents code from returning '000.0'
            else
                lhs = (decimal<=0)? (decimal)%3 + 3  :  (decimal-1)%3+1;
            useExponent = !(lhs==decimal);   // only use exponent if we really need it
            break;

        default:
            return "**bad mode**";
    }

    D2TAG("*** lhs is", itos(lhs));

    // Figure out the number of digits to show in the right hand side
    int rhs=0;
    if (precision>=0) 
        rhs = precision;
    else if (val == 0.0)
        rhs = 0;
    else if (useExponent || decimal>0) 
        rhs = dig-lhs;
    else
        rhs = dig-decimal;

    // can't use a negative rhs value, so turn it to zero if that is the case
    if (rhs<0) rhs = 0;      
    D2TAG("*** rhs is", itos(rhs));

    // Determine the exponent
    int exponent = decimal - lhs;
    if (val==0.0) exponent=0;          // prevent zero from getting an exponent
    D2TAG("*** exponent is", itos(exponent));
    
    string ascii;
    
    // output the sign
    if (sign) ascii += "-";
    else if (forcePlus) ascii += "+";

    // output the left hand side
    if (!useExponent && decimal<=0)    // if fraction, put the 0 out front
        ascii += '0';                  
    else                               // is either exponential or >= 1, so write the lhs
        for (; lhs>0; lhs--)
            ascii += (*p)? *p++ : int('0'); // now fill in the numbers before decimal
    D2TAG("*** ascii + sign + lhs is", ascii);

    // output the decimal point
    if (forceDecimal || rhs>0)
        ascii += '.';

    // output the right hand side
    if (!useExponent && rhs>0)         // first fill in zeros after dp and before numbers
        while (decimal++ <0 && rhs-->0)
            ascii += '0';              
    for (; rhs>0 ; rhs--)              // now fill in the numbers after decimal
        ascii += (*p)? *p++ : int('0');     
    D2TAG("*** ascii + . + rhs is", ascii);

    if (forceExponent || useExponent)  // output the entire exponent if required
    {
        ascii += E;                    // output the E or X
        if (forcex10) ascii += "10^";  // if using 'x10^' format, output the '10^' part

        // output the exponent's sign
        if (exponent < 0) {            // Negative exponent
            exponent = -exponent;      // make exponent positive if needed
            ascii += '-';              // output negative sign
        }
        else if (forceExpPlus)         // We only want the '+' if it is asked for explicitly
            ascii += '+';

        // output the exponent
        if (forceHundredExpZero || exponent >= 100)
            ascii += ( (exponent/100) % 10 + '0' );
        if (forceHundredExpZero || forceExpZero || exponent >= 10)
            ascii += ( (exponent/10) % 10 + '0' );
        ascii += ( exponent % 10 + '0' );
        D2TAG("*** ascii + exp is", ascii);
    }

    D2TAG("*** End of ftos with ascii = ", ascii);
    /* finally, we can return */
    return ascii;
}

#ifdef TESTFTOS
#include <fstream.h>
#include <curses.h>

void test(double val, char mode, int sigfig, int prec, int options, const string& t)
{
    string s = ftos(val,mode,sigfig,prec,options);
#ifdef CONSOLE
#else
    cout << "'" << mode << "' = (" << val << ") = [" << s << "] = " << t << endl;
#endif
    if ( s!=t && !(s=="" && t=="NULL") ) {
#ifdef CONSOLE
#else
        cout << endl << val << ", " << mode << ", " << sigfig << ", " << prec << ", " 
             << options << endl;
        cout << "<> ERROR:  Bad match!!! Should be <" << t << ">" << endl;
        cout << "Hit a key" << endl;
#endif
        getch();
    }
}

void filetest()
{
    double x;
    string xs;
    int sf;
    int p;
    int options;
    char mode;
    string ts;
    ifstream ifile("ftostest.in");
    ofstream ofile("ftostest.dat");

    while (ifile) {
        ifile >> xs >> mode >> sf >> p >> options >> (x = atof(xs.c_str()));
        test(x, mode, sf, p, options, ts);
        ofile << xs << " " << mode    << " " << sf << " " 
              << p  << " " << options << " " << ts << endl;
    }
}

void makeTestFile()
{
    double val[] = { 
        0, 1, -1, 0.03555555556, 123456.0, 9.8765e10, -.00000383838, -43.56
    };
    int sigfig[] = {
        -4, -1, 0, 1, 4, 6, 9, 16
    };
    int prec[] = {
        -4, -1, 0, 1, 2, 4, 6, 10
    };
    char mode[] = {
        'e', 'f', 'g', 'h'
    };
    ofstream ofile("ftostest.in");

    for (int i=0; i<8; i++)
        for (int j=0; j<4; j++)
            for (int k=0; k<8; k++)
                for (int m=0; m<8; m++)
                    for (int options=0; options<(1<<7); options++) {
                        string s = ftos(val[i],mode[j],sigfig[k],prec[m],options);
                        if (s=="") s="NULL";
                        ofile << ftos(val[i])  << " " << mode[j] << " " << sigfig[k] << " " 
                              << prec[m] << " " << options << " " << s  << endl;
                    }
}

int main()
{
    makeTestFile();
    filetest();

    return 0;
}

#endif // TESTFTOS

////////////////////////////////////////////////////////////////////////
// round.cc
// Copyright (c) 1996 Bryce W. Harrington
// - bryce@netcom.com - bryce@cco.caltech.edu - bryce@python.sp.trw.com 
// = http://www.cco.caltech.edu/~bryce = http://www.genealogy.org/~bryce
//-----------------------------------------------------------------------
// License:  This code may be used by anyone for any purpose
//           so long as the copyright notices and this license
//           statement remains attached.
//-----------------------------------------------------------------------
// This routine rounds a double using the "rounding rule", as expressed
// in _Advanced Engineering Mathematics_ by Erwin Kreyszig, 6th ed., 
// John Wiley & Sons, Inc., 1988, page 945.
//
// Discard the (k+1)th and all subsequent decimals.
//  (a) If the number thus discarded is less than half a unit in the
//      kth place, leave the kth decimal unchanged ("rounding down")
//  (b) If it is greater than half a unit in the kth place, add one
//      to the kth decimal ("rounding up")
//  (c) If it is exactly half a unit, round off to the nearest *even* 
//      decimal.
//  Example:  Rounding off 3.45 and 3.55 by one decimal gives 3.4 and
//      3.6, respectively.
//  Rule (c) is to ensure that in discarding exactly half a decimal,
//      rounding up and rounding down happens about equally often,
//      on the average.
///////////////////////////////////////////////////////////////////////
#include <math.h>

double round(double x)
{
	double xlow = floor(x);
	if (x - xlow != 0.5000)
		return floor(x + 0.5000);
	else if ( floor(x/2.0) == xlow/2.0)
		return xlow;
	else
		return xlow++;		
}

// This version allows rounding to a specific digit
double round(double x, int k)
{
	if (k==0) return round(x);
	else return round(x*pow(10,k)) / pow(10,k);
}

/////////////////////////////////////////////////////////////////////////
// itos.cc
// Copyright (c) 1996 Bryce W. Harrington
// - bryce@netcom.com - bryce@cco.caltech.edu - bryce@python.sp.trw.com 
// = http://www.cco.caltech.edu/~bryce = http://www.genealogy.org/~bryce
//-----------------------------------------------------------------------
// License:  This code may be used by anyone for any purpose
//           so long as the copyright notices and this license
//           statement remains attached.
//-----------------------------------------------------------------------
// This routine converts an integer into a string

// Standard include files
#include <string>		// for string
#include <algorithm>		// for reverse

// Site specific include files (X11, Gnu, etc.)
//#include <bstring.h>

// BH include files

// Project specific include files

string itos(int n)
{
  int sign;
  string s;

  if ((sign = n) < 0)           // record sign
    n = -n;                     // make n positive
  do {                          // generate digits in reverse order
    s += (char(n % 10) + '0');   // get next digit
  } while ((n/=10) > 0);        // delete it

  if (sign < 0)
    s += '-';

  reverse(s.begin(), s.end());  // This is what the code should look like
                                // if the string class is compatible with
                                // the standard C++ string class
#ifdef DUMB_OS_LIKE_WINDOWS
  // In Windows, we'll use this hack...
  for (int i=0, j=s.GetLength()-1; i<j; i++, j--)
  {
	  char c = s[i];
//	  s[i] = s[j];
//	  s[j] = c;
      s.SetAt(i, s[j]);
      s.SetAt(j, c);
  }
#endif

  return s;
}

