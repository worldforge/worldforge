/*
 *  Config.h - interface for configuration class
 *  Copyright (C) 2000, Stefanus Du Toit, Joseph Zupko
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
 
#ifndef VARCONF_CONFIG_H
#define VARCONF_CONFIG_H

#include <map>
#include <multimap.h>
#include <string>

#include <sigc++/signal_system.h>
#include "parse_error.h"
#include "variable.h"

namespace varconf {

typedef std::map< std::string, std::map< std::string, Variable > > conf_map;
typedef std::map< std::string, Variable > sec_map;
typedef std::map< char, pair<std::string, bool> > parameter_map;

class Config {
public:
  static Config* inst();
  // Precondition: none.
  // Postcondition: a pointer to the "global configuration" instance 
  //                is returned; this should be used when only one 
  //                collection of configuration data is required.

  void clean( std::string& str);
  // Precondition: any valid string.
  // Postcondition: string is formatted for use in section and key names; 
  //                any invalid characters (not A-Z, a-z, 0-9, - or _)
  //                are replaced with the _ character and all letters
  //                are made lowercase.

  bool erase( const std::string& section, const std::string& key = ""); 
  // Precondition: an existing section or key.
  // Postcondition: section or key is erased from configuration data; false
  //                is returned if section or key does not exist.

  bool find( const std::string& section, const std::string& key = "");
  // Precondition: a valid section or key name.
  // Postcondition: section or key is located and true is returned if found.

  bool findItem( const std::string& section, const std::string& key)
  {
    return find( section, key);
  } 
  // Same functionality as key search with find method.  Here for backwards
  // compatability.

  void getCmdline( int argc, char** argv);
  // Precondition: an array of command-line arguments along with an int value of
  //               the number of arguments.
  // Postcondition: command-line arguments are parsed and stored in memory;       // warnings may be sent to the error stream.

  void getEnv( const std::string& prefix);
  // Precondition: a valid string.
  // Postcondition: all environment variables with the passed prefix are parsed   // and stored in memory; this prefix is currently case-sensitive; environment
  // variables do not currently support section names.

  Variable getItem( const std::string& section, const std::string& key);
  // Precondition: a valid key.
  // Postcondition: data stored in the passed key is returned in a Variable data  // type; see variable.h and variable.cpp for more information.

  void parseStream( std::istream& in) throw ( ParseError);
  // Precondition: valid input stream to read configuration data from.
  // Postcondition: configuration data is read from the input stream
  //                and stored in memory; a 'ParseError' object is thrown
  //                when parsing errors are encountered.

  bool readFromFile( const std::string& filename);
  // Precondition: valid filename of an existing configuration file.
  // Postcondition: configuration data in the passed configuration file is
  //                parsed and stored; 'false' is returned if the file cannot
  //                be opened; any parsing errors are reported to 'cerr'. 

  void setItem( const std::string& section, const std::string& key, 
                const Variable& item);
  // Precondition: a section and key name and its contents stored in a Variable   //               type.
  // Postcondition: configuration item of specified properties is created in      //                memory.

  void setParameterLookup( char s_name, const std::string& l_name, 
                           bool value = false);
  // Precondition: a one-character command-line argument, its full-name 
  //               equivalent and a bool indicating whether it is followed
  //               by a value or not.
  // Postcondition: the one-character command-line argument is attached to its
  //                long-name; this long-name is then stored in memory in 
  //                place of the one-character short-name.
  
  bool writeToFile( const std::string& filename);
  // Precondition: valid filename of an output file to write to.
  // Postcondition: configuration data stored in memory is written to 
  //                the passed file; 'false' is returned if the file cannot
  //                be opened.
    
  bool writeToStream( std::ostream& out);
  // Precondition: valid output stream to write to.
  // Postcondition: configuration data stored in memory is written to the
  //                passed output stream; 'true' is currently returned
  //                under all circumstances.

  SigC::Signal0<void> sig;
  // standard signal - libsigc++ signal that informs of a change but
  // not what was changed.  See libsigc++ docs for details.
 
  SigC::Signal2<void, const std::string&, const std::string&> sigv;
  // verbose signal - libsigc++ signal that informs of a change and
  // returns first the 'section' and then the 'name' of the configuration 
  // item changed.  See libsigc++ docs for details.
 
protected:
  Config();
  Config( const Config& config);
  Config& operator=( const Config&);
  
private:
  conf_map m_conf;
  static Config* m_instance;
  parameter_map m_par_lookup;
};

} // namespace varconf
#endif





