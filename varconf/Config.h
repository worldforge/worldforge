/*
 *  Config.h - interface for configuration class
 *  Copyright (C) 2001, Stefanus Du Toit, Joseph Zupko
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Contact:  Joseph Zupko
 *            jaz147@psu.edu
 *
 *            189 Reese St.
 *            Old Forge, PA 18518
 */
 
#ifndef VARCONF_CONFIG_H
#define VARCONF_CONFIG_H

#include <iostream>
#include <map>
#include <string>

#include <sigc++/signal_system.h>
#include <varconf/parse_error.h>
#include <varconf/variable.h>

namespace varconf {

typedef std::map< std::string, std::map< std::string, Variable > > conf_map;
typedef std::map< char, std::pair<std::string, bool> > parameter_map;
typedef std::map< std::string, Variable > sec_map;

class Config : virtual public SigC::Object {
public:
  static Config* inst();
  // Allows use as a singleton, if desired.

  Config() { }

  Config( const Config& conf);
  // New Config object, but deep-copies the m_conf and m_par_lookup of existing,
  // passed Config object.

  virtual ~Config() {if(m_instance == this) m_instance = 0;}

  friend std::ostream& operator <<( std::ostream& out, Config& conf);
  friend std::istream& operator >>( std::istream& in, Config& conf);
  friend bool operator ==( const Config& one, const Config& two);

  void clean( std::string& str);
  // Converts all nonalphanumeric characters in str except ``-'' and ``_'' to
  // ``_''; converts caps in str to lower-case.

  bool find( const std::string& section, const std::string& key = "");
  // Returns true if specified key exists under specified section.

  bool erase( const std::string& section, const std::string& key = ""); 
  // Returns true if specified key exists under specified section and is
  // successfully deleted.

  bool writeToStream( std::ostream& out);
  // Writes to the specified output stream.
  // Why isn't this protected?

  void getCmdline( int argc, char** argv);
  // Gets, sets conf info based on options passed via command line.

  void getEnv( const std::string& prefix); 
  // Gets, stores a name/value pair from the environment variable with 
  // name == prefix.
  // prefix is case-sensitive!

  bool writeToFile( const std::string& filename);
  // Writes conf map to specified file.

  bool readFromFile( const std::string& filename);
  // Reads contents of specified file and set into conf map.

  void parseStream( std::istream& in) throw ( ParseError);
  // Ensures specified filestream is properly formatted.
  // Why isn't this protected?

  bool findItem( const std::string& section, const std::string& key);
  // Wrapper for find(section, key)

  Variable getItem( const std::string& section, const std::string& key);
  // Returns value of specified key under specified section.

  void setParameterLookup( char s_name, const std::string& l_name, bool value = false); 

  void setItem( const std::string& section, const std::string& key, const Variable& item); 
  // If key isn't null, clean() section and key and set variable.
 
  SigC::Signal0<void> sig;
  SigC::Signal1<void, const char*> sige;
  SigC::Signal2<void, const std::string&, const std::string&> sigv; 
  SigC::Signal3<void, const std::string&, const std::string&, Config&> sigsv;
  // libsigc++ signals; in order: standard callback signal, error signal,
  // verbose callback signal and "super-verbose" callback signal. 

private:
  static Config* m_instance;
  conf_map m_conf;
  parameter_map m_par_lookup;
};

} // namespace varconf
#endif

