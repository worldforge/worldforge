Varconf - STAGE Configuration System (c)1999-2003


1. Overview
-----------
Varconf is a configuration system designed for the STAGE server.
Varconf can parse configuration files, command-line arguments and environment
variables.  It supports callbacks and can store its configuration information
in separate Config objects or in one global configuration instance.


2. Disclaimer
-------------
None of the authors of this package nor anyone involved with the World Forge
project is liable for any damages caused by the use of varconf.  If your
computer, you, or anyone around you self-combusts as a result of running this
program, locate the nearest fire-extinguisher and don't call us. :)


3. Installation
---------------
The libsigc++ callback library needs to be compiled and installed before
compiling varconf. The library can be aquired at

http://sourceforge.net/projects/libsigc/.

This version requires SigC++ 1.2 series and pkgconfig. If you want to use 
SigC++ 1.0 series, or the legacy varconf-config script, please try a
version of varconf from the 0.5 series.


3.1 Basic Installation
----------------------
cd forge/libs/varconf
sh autogen.sh 
./configure
make
make install

This will place the libraries in /usr/local/lib/varconf
and the header files in /usr/local/include/varconf


4. Definitions
--------------
Configuration data refers to varconf's internal handling of section names,
item names and item values.  Each of these components has the same meaning
as they would in any standard configuration file.  Each of these components
comes from configuration files, environment variables or command-line
arguments.


4.1 Configuration Data 
-----------------------
The configuration is a database that consists of sections.

Sections contain configuration items and each section has a section identifier.

A section identifier is a lower case name, consisting of the characters
'a'-'z', '0'-'9', '-' and '_'.

A configuration item consists of a name and a value.

A configuration item name is a lower-case identifier, consisting of the
characters 'a'-'z', '0'-'9', '-' and '_'.

A configuration item value is a piece of information stored internally as a
string value that may be represented as at least one of the configuration
types.

The configuration types include
 - boolean, i.e. 2 possible values, either true or false
 - integer, i.e. a positive or negative whole number
 - double, i.e. a double-precision positive or negative real number
 - string, i.e. a free-form string of characters including all characters in
   the ASCII character set from characters 32-255


4.2 Configuration File Format
-----------------------------
A line in a configuration file can either be the start of a section, a
configuration item or a comment.

\n denotes the end of a line or the end of the file
{ws} denotes any (0 or more) amount of whitespace
{nm} denotes a name consisting of a-z, A-Z, 0-9, '_' or '-'
{ac} denotes any character other than '"'
{cm} denotes any character other than \n

Syntax for section start:

{ws}[{nm}]{ws}\n

Syntax for configuration item:

{ws}{nm}{ws}={ws}{nm{ws}\n
  or
{ws}{nm}{ws}={ws}"{ac}"{ws}\n

Syntax for comment:

{ws}#{cm}\n

Note that upper case characters (A-Z) in configuration item names and section
names get converted to their lower case equivalents (a-z) automatically.

A configuration item may be followed by a comment.

Empty lines (lines with 0 or more whitespaces and no other characters) are
ignored.


4.3 - Command-Line Arguments
----------------------------
A single command-line argument can be a short-form argument or short-form
argument value, a long-form argument or ignored data.

{sn} denotes a short-name of one of either 'a'-'z', 'A'-'Z' or '0'-'9'.
{nm} denotes a name consisting of 'a'-'z', 'A'-'Z', '0'-'9', '_' or '-'.
{ac} any character other than '"'.

Syntax for short-form argument:

-{sn}

Syntax for short-form argument value:

{nm}
  or
"{ac}"

Syntax for long-form argument:

--{nm}:{nm}={nm}
    or                    --{section_name}:{item_name}={item_value}
--{nm}:{nm}="{ac}"


4.4 - Environment Variables
---------------------------
To allow varconf to input an environmental variable as configuration data,
precede its name with a prefix and then specify that prefix when calling
the function. ie:

Variables:
  WF_ONE=Oh
  WF_TWO=Happy
  WF_THREE=Day

Use prefix 'WF_' to retrieve the three variables as configuration data.
They will be stored as items 'one', 'two' and 'three' respectively.


5. Library Usage
----------------
Varconf is not very verbose about errors. Typically, malformed config data
is either completely ignored or converted into a compatible form.
A typo won't crash the library but it might lead to some very odd
configuration items. ie:

Command-line argument:
  --section_name::    second colon treated as item name and converted to
                      '_'; item '_' would be created without value under
                      section "section_name"

While this makes reporting typos to the user difficult, it makes parsing
of configuration data more efficient in the library.  A future solution to
this might be the addition of warnings when the configuration data set method
receives erroneous data. 


5.1 Configuration Instance
--------------------------
Configuration data can be stored in separate configuration objects or in a
single global instance.  

Separate Objects:
 
  Config my_config;

Global Instance:

  *Config::inst()

Note that the method Config::inst() will automatically initialize the
global data if not already created.


5.2 libsigc++ 
-------------
Varconf uses libsigc++ for its callback routines and error reporting.
Please see the libsigc++ documentation for specific details on interacting
with libsigc++ signals. 

Four libsigc++ signals are provided by varconf.

  .sig - callback method with no details. Indicates that configuration data
         has been changed but does not specify what has been changed.
	 
  .sigv - callback method that reports section and item name that
          has been changed. Only useful when working with a single
	  configuration instance.A
	  
  .sigsv - callback method that reports section, item name and the address of
           the configuration object under which the change was made. Useful
	   when using multiple configuration instances.
 
  .sige - reports error messages in a single string. 


5.3 Configuration Files
-----------------------
Varconf can read file-formed config data either directly from a file or from
any input stream. It can also write configuration data directly to a file
or to an output stream.

To read from a file:
 
  config_object.readFromFile( file_name);

To read from an input stream:

  config_object.parseStream( istream&);

  Note that this method may throw an error of type ParseError. 
  See the parse_error.cpp file or the parse_error.h file for more information.


To write to a file:

  config_object.writeToFile( file_name);

To write to a stream:

  config_object.writeToStream( ostream&);


5.4 Command-line Arguments
--------------------------
To parse a set of command-line arguments call the following method with the
number of arguments and multi-dimension array of command-line arguments.

  config_object.getCmdline( int argc, char** argv);

Note that before shortname arguments ('-a') can be used, each must be
registered with a long name using the setParameterLookup method. ie:

  config_object.setParameterLookup( 'f', "foo", true);

The last argument is a bool specifying whether the argument requires a value
or not. If true, varconf will look at the command-line argument directly
following the shortform argument to see if there is a value there. If false,
varconf will store the argument with no value.

Note that if a shortform argument is found but does not exist in the table,
it will be ignored without warning.


5.5 Environment Variables
-------------------------
To read in all environment variables with a specific prefix, call the
following function with that prefix:

  config_object.getEnv( prefix);

  ie: config_object.getEnv( "WF_");    reads in all environment variables
                                       starting with "WF_"
