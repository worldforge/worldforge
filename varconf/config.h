/*
 *  config.h - interface for configuration class
 *  Copyright (C) 2001, Stefanus Du Toit, Joseph Zupko
 *            (C) 2002-2006 Alistair Riddoch
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

#include <varconf/varconf_defs.h>
#include <varconf/parse_error.h>
#include <varconf/variable.h>

#include <sigc++/trackable.h>
#include <sigc++/signal.h>

#include <iostream>
#include <map>
#include <string>

namespace varconf {

typedef std::map< std::string, Variable > sec_map;
typedef std::map< std::string, sec_map> conf_map;
typedef std::map< char, std::pair<std::string, bool> > parameter_map;

class VARCONF_API Config : virtual protected sigc::trackable {
public:
  // Allows use as a singleton, if desired.
  static Config* inst();

  Config() { }

  // New Config object, but deep-copies the m_conf and m_par_lookup of existing,
  // passed Config object.
  Config(const Config & conf);

  virtual ~Config();

  friend std::ostream & operator <<(std::ostream & out, Config & conf);
  friend std::istream & operator >>(std::istream & in, Config & conf);
  friend bool operator ==(const Config & one, const Config & two);

  // Converts all nonalphanumeric characters in str except ``-'' and ``_'' to
  // ``_''; converts caps in str to lower-case.
  static void clean(std::string & str);

  // Returns true if specified key exists under specified section.
  bool find(const std::string & section, const std::string & key = "") const;

  // Returns true if specified key exists under specified section and is
  // successfully deleted.
  bool erase(const std::string & section, const std::string & key = ""); 

  // Writes to the specified output stream.
  // Why isn't this protected?
  bool writeToStream(std::ostream & out, Scope scope_mask) const;

  // Gets, sets conf info based on options passed via command line.
  int getCmdline(int argc, char** argv, Scope scope = INSTANCE);

  // Gets, stores a name/value pair from the environment variable with 
  // name == prefix.
  // prefix is case-sensitive!
  void getEnv(const std::string & prefix, Scope scope = INSTANCE); 

  // Writes conf map to specified file. Returns true on success.
  bool writeToFile(const std::string & filename,
                   Scope scopeMask = (Scope)(GLOBAL | USER | INSTANCE)) const;

  // Reads contents of specified file and set into conf map. Returns
  // true on success.
  bool readFromFile(const std::string & filename, Scope scope = USER);

  // Ensures specified filestream is properly formatted.
  // Why isn't this protected?
  void parseStream(std::istream & in, Scope scope) throw (ParseError);

  // Wrapper for find(section)
  bool findSection(const std::string & section) const;

  // Wrapper for find(section, key)
  bool findItem(const std::string & section, const std::string & key) const;

  // Returns value of specified section.
  const sec_map & getSection(const std::string & section);
 
  // Returns value of specified key under specified section.
  Variable getItem(const std::string & section, const std::string & key) const;

  // Set the short name for a given long name to be used with short args.
  void setParameterLookup(char s_name, const std::string & l_name,
                          bool value = false); 

  // If key isn't null, clean() section and key and set variable.
  void setItem(const std::string & section, const std::string & key,
               const Variable & item, Scope scope = INSTANCE); 

  // Accessor for the contained sections.
  const conf_map& getSections() const;
 
  sigc::signal<void> sig;
  sigc::signal<void, const char*> sige;
  sigc::signal<void, const std::string&, const std::string&> sigv; 
  sigc::signal<void, const std::string&, const std::string&, Config&> sigsv;
  // libsigc++ signals; in order: standard callback signal, error signal,
  // verbose callback signal and "super-verbose" callback signal. 

private:
  static Config* m_instance;
  conf_map m_conf;
  parameter_map m_par_lookup;
};


VARCONF_API std::ostream & operator <<(std::ostream & out, Config & conf);
VARCONF_API std::istream & operator >>(std::istream & in, Config & conf);
VARCONF_API bool operator ==(const Config & one, const Config & two);

} // namespace varconf

#endif // VARCONF_CONFIG_H
