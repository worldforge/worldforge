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
#include "parse_error.h"
#include "variable.h"

namespace varconf {

typedef std::map< std::string, std::map< std::string, Variable > > conf_map;
typedef std::map< char, pair<std::string, bool> > parameter_map;
typedef std::map< std::string, Variable > sec_map;

class Config {
public:
  static Config* inst();

  Config() { }
  Config( const Config& conf);

  virtual ~Config() { }

  friend ostream& operator <<( ostream& out, Config& conf);
  friend istream& operator >>( istream& in, Config& conf);
  friend bool operator ==( const Config& one, const Config& two);

  void clean( std::string& str);
  bool find( const std::string& section, const std::string& key = "");
  bool erase( const std::string& section, const std::string& key = ""); 

  bool writeToStream( std::ostream& out);
  void getCmdline( int argc, char** argv);
  void getEnv( const std::string& prefix); // prefix is case-sensitive!
  bool writeToFile( const std::string& filename);
  bool readFromFile( const std::string& filename);
  void parseStream( std::istream& in) throw ( ParseError);
  bool findItem( const std::string& section, const std::string& key);
  Variable getItem( const std::string& section, const std::string& key);
  void setParameterLookup( char s_name, const std::string& l_name, bool value = false); 
  void setItem( const std::string& section, const std::string& key, const Variable& item); 
  
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





