/*
 *  config.cpp - implementation of the main configuration class.
 *  Copyright (C) 2001, Stefanus Du Toit, Joseph Zupko
 *            (C) 2003-2006 Alistair Riddoch
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
 
#include <varconf/config.h>

#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>

#ifdef __WIN32__
#include <tchar.h>
#define snprintf _snprintf
#include <cstdlib>
#else // __WIN32__

extern char** environ;


// on OS-X, the CRT doesn't expose the environ symbol. The following
// code (found on Google) provides a value to link against, and a
// further tweak in getEnv gets the actual value using _NS evil.
#if defined(__APPLE__)
    #include <crt_externs.h>
    char **environ = NULL;
#endif

#endif // __WIN32__

namespace {
  enum state_t {
    S_EXPECT_NAME, // Expect the start of a name/section/comment
    S_SECTION, // Parsing a section name
    S_NAME, // Parsing an item name
    S_COMMENT, // Parsing a comment
    S_EXPECT_EQ, // Expect an equal sign
    S_EXPECT_VALUE, // Expect the start of a value
    S_VALUE, // Parsing a value
    S_QUOTED_VALUE, // Parsing a "quoted" value
    S_EXPECT_EOL // Expect the end of the line
  };

  enum ctype_t {
    C_SPACE, // Whitespace
    C_NUMERIC, // 0-9
    C_ALPHA, // a-z, A-Z
    C_DASH, // '-' and '_'
    C_EQ, // '='
    C_QUOTE, // '"'
    C_SQUARE_OPEN, // '['
    C_SQUARE_CLOSE, // ']'
    C_HASH, // '#'
    C_ESCAPE, // '\' (an "escape")
    C_EOL, // End of the line
    C_OTHER // Anything else
  };

  ctype_t ctype(char c)
  {
    if (c=='\n') return C_EOL;
    if (isspace(c)) return C_SPACE;
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) return C_ALPHA;
    if (isdigit(c)) return C_NUMERIC;
    if (c == '-' || c == '_') return C_DASH;
    if (c == '=') return C_EQ;
    if (c == '"') return C_QUOTE;
    if (c == '[') return C_SQUARE_OPEN;
    if (c == ']') return C_SQUARE_CLOSE;
    if (c == '#') return C_HASH;
    if (c == '\\') return C_ESCAPE;
    return C_OTHER;
  }
}

namespace varconf {

Config* Config::m_instance = 0;

Config* Config::inst()
{
  if (m_instance == NULL) 
    m_instance = new Config;
  
  return m_instance;
}

Config::Config(const Config & conf)
{
  m_conf = conf.m_conf;
  m_par_lookup = conf.m_par_lookup;
}

Config::~Config()
{
  if (m_instance == this)
    m_instance = NULL;
}

std::ostream & operator <<(std::ostream & out, Config & conf)
{
  if (!conf.writeToStream(out, USER)) {
    conf.sige.emit("\nVarconf Error: error while trying to write "
                    "configuration data to output stream.\n");
  }
 
  return out;
}

std::istream & operator >>(std::istream & in, Config & conf)
{
  try {
    conf.parseStream(in, USER);
  }
  catch (ParseError p) {
    char buf[1024];
    std::string p_str = p;
    snprintf(buf, 1024, "\nVarconf Error: parser exception throw while "
                         "parsing input stream.\n%s", p_str.c_str());   
    conf.sige.emit(buf);
  }

  return in;
} 

bool operator ==(const Config & one, const Config & two)
{
  if (one.m_conf == two.m_conf && one.m_par_lookup == two.m_par_lookup) {
    return true;
  } else {
    return false;
  }
}

void Config::clean(std::string & str)
{
  ctype_t c;

  for (size_t i = 0; i < str.size(); i++) {
    c = ctype(str[i]);

    if (c != C_NUMERIC && c != C_ALPHA && c != C_DASH) {
      str[i] = '_';
    } else {
      str[i] = (char) tolower(str[i]);
    }
  } 
}

bool Config::erase(const std::string & section, const std::string & key)
{
  if (find(section)) {
    if (key == "") {
      m_conf.erase(section);
      return true;
    } else if (find(section, key)) {
      m_conf[section].erase(key);
      return true;
    }
  }
 
  return false;
}

bool Config::find(const std::string & section, const std::string & key) const
{
  conf_map::const_iterator I = m_conf.find(section);
  if (I != m_conf.end()) {
    if (key == "") {
      return true;
    } 
    const sec_map & sectionRef = I->second;
    sec_map::const_iterator J = sectionRef.find(key);
    if (J != sectionRef.end()) {
      return true;
    }
  }

  return false;
}

bool Config::findSection(const std::string & section) const
{
  return find(section);
}

bool Config::findItem(const std::string & section, const std::string & key) const
{
  return find(section, key);
}

int Config::getCmdline(int argc, char** argv, Scope scope)
{
  int optind = 1;

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-' ) {
       continue;
    }

    std::string section, name, value, arg;
    bool fnd_sec = false, fnd_nam = false;
    size_t mark = 2;
    if (argv[i][1] == '-' && argv[i][2] != '\0') {
      // long argument
      arg = argv[i];
       
      for (size_t j = 2; j < arg.size(); j++) {
        if (arg[j] == ':' && arg[j+1] != '\0' && !fnd_sec && !fnd_nam) {
          section = arg.substr(mark, (j - mark));
          fnd_sec = true;
          mark = j + 1;
        }
        else if (arg[j] == '=' && (j - mark) > 1) {
          name = arg.substr(mark, (j - mark));
          fnd_nam = true;
          value = arg.substr((j + 1), (arg.size() - (j + 1)));
          break;
        }
      }

      if (!fnd_nam && (arg.size() - mark) > 0)  {
        name = arg.substr(mark, (arg.size() - mark));
      }

    } else if (argv[i][1] != '-' && argv[i][1] != '\0') {
      // short argument
      parameter_map::iterator I = m_par_lookup.find(argv[i][1]);

      if (I != m_par_lookup.end()) {
        name = ((*I).second).first;
        bool needs_value = ((*I).second).second;

        if (needs_value && (i+1) < argc && argv[i+1][0] != 0
                        && argv[i+1][0] != '-') {
          value = argv[++i];
        } 
        else {
          char buf[1024];
          snprintf(buf, 1024, "\nVarconf Warning: short argument \"%s\""
                               " given on command-line expects a value"
                               " but none was given.\n", argv[i]); 
          sige.emit(buf);
        }
      }
      else {
        char buf[1024];
        snprintf(buf, 1024, "\nVarconf Warning: short argument \"%s\""
                             " given on command-line does not exist in"
                             " the lookup table.\n", argv[i]);
        sige.emit(buf);
      }
    }

    if (!name.empty()) {
      setItem(section, name, value, scope);
      optind = i + 1;
    }
  }
  return optind;
}

void Config::getEnv(const std::string & prefix, Scope scope)
{
  std::string name = "", value = "", section = "", env = "";
  size_t eq_pos = 0;

#if defined(__APPLE__)
  if (environ == NULL)
      environ = *_NSGetEnviron();
#endif
  
  for (size_t i = 0; environ[i] != NULL; i++) {
    env = environ[i];

    if (env.substr(0, prefix.size()) == prefix) {
      eq_pos = env.find('='); 

      if (eq_pos != std::string::npos) {
        name = env.substr(prefix.size(), (eq_pos - prefix.size()));
        value = env.substr((eq_pos + 1), (env.size() - (eq_pos + 1)));
      }
      else {
        name = env.substr(prefix.size(), (env.size() - prefix.size()));
        value = "";
      }   
      
      setItem(section, name, value, scope);
    }
  }
}

const sec_map & Config::getSection(const std::string & section)
{
  // TODO: This will create a new section in the config file. Is really the 
  //  desired behaviour?
  return m_conf[section];
}

Variable Config::getItem(const std::string & section, const std::string & key) const
{
  conf_map::const_iterator I = m_conf.find(section);
  if (I != m_conf.end()) {
    std::map<std::string, Variable>::const_iterator J = I->second.find(key);
    if (J != I->second.end()) {
      return J->second;
    }
  }
  return Variable();
}

const conf_map& Config::getSections() const
{
	return m_conf;
}


void Config::parseStream(std::istream & in, Scope scope) throw (ParseError)
{
  char c; 
  bool escaped = false;
  size_t line = 1, col = 0;
  std::string name = "", value = "", section = "";
  state_t state = S_EXPECT_NAME;

  while (in.get(c)) {
    col++;
    switch (state) {
      case S_EXPECT_NAME : 
        switch (ctype(c)) {
          case C_ALPHA:
          case C_NUMERIC:
          case C_DASH:
            state = S_NAME;
            name = c;
            break;
          case C_SQUARE_OPEN:
            section = "";
            state = S_SECTION;
            break;
          case C_SPACE:
          case C_EOL:
            break;
          case C_HASH:
            state = S_COMMENT;
            break;
          default:
            throw ParseError("item name", (int) line, (int) col);
            break;
        }
        break;
      case S_SECTION :
        switch (ctype(c)) {
          case C_ALPHA:
          case C_NUMERIC:
          case C_DASH:
            section += c;
            break;
          case C_SQUARE_CLOSE:
            state = S_EXPECT_EOL;
            break;
          default:
            throw ParseError("']'", (int) line, (int) col);
            break;
        }
        break;
      case S_NAME :
        switch (ctype(c)) {
          case C_ALPHA:
          case C_NUMERIC:
          case C_DASH:
            name += c;
            break;
          case C_EQ:
            state = S_EXPECT_VALUE;
            break;
          case C_SPACE:
            state = S_EXPECT_EQ;
            break;
          default:
            throw ParseError("'='", (int) line, (int) col);
            break;
        }
        break;
      case S_COMMENT :
        switch (ctype(c)) {
          case C_EOL:
            state = S_EXPECT_NAME;
            break;
          default:
            break;
        }
        break;
      case S_EXPECT_EQ:
        switch (ctype(c)) {
          case C_SPACE:
            break;
          case C_EQ:
            state = S_EXPECT_VALUE;
            break;
          default:
            throw ParseError("'='", (int) line, (int) col);
            break;
        }
        break;
      case S_EXPECT_VALUE:
        switch (ctype(c)) {
          case C_ALPHA:
          case C_NUMERIC:
          case C_DASH:
            state = S_VALUE;
            value = c;
            break;
          case C_QUOTE:
            value = "";
            state = S_QUOTED_VALUE;
            break;
          case C_EOL:
            value = "";
            state = S_EXPECT_NAME;
            setItem(section, name, value, scope);
            break;
          case C_SPACE:
            break;
          default:
            throw ParseError("value", (int) line, (int) col);
            break;
        }
        break;
      case S_VALUE:
        switch (ctype(c)) {
          case C_QUOTE:
            throw ParseError("value", (int) line, (int) col);
          case C_SPACE:
            state = S_EXPECT_EOL;
            setItem(section, name, value, scope);
            break;
          case C_EOL:
            state = S_EXPECT_NAME;
            setItem(section, name, value, scope);
            break;
          case C_HASH:
            state = S_COMMENT;
            setItem(section, name, value, scope);
            break;
          default:
            value += c;
            break;
        }
        break;
      case S_QUOTED_VALUE:
        if (escaped) {
          value += c;
          escaped = false;
        } else {
          switch (ctype(c)) {
            case C_QUOTE:
              state = S_EXPECT_EOL;
              setItem(section, name, value, scope);
              break;
            case C_ESCAPE:
              escaped = true;
              break;
            default:
              value += c;
              break;
          }
        }
        break;
      case S_EXPECT_EOL:
        switch (ctype(c)) {
          case C_HASH:
            state = S_COMMENT;
            break;
          case C_EOL:
            state = S_EXPECT_NAME;
            break;
          case C_SPACE:
            break;
          default:
            throw ParseError("end of line", (int) line, (int) col);
            break;
        }
        break;
      default:
        break;
    }
    if (c == '\n') {
      line++;
      col = 0;
    }
  } // while (in.get(c))

  if (state == S_QUOTED_VALUE) {
    throw ParseError("\"", (int) line, (int) col);
  }

  if (state == S_VALUE) {
    setItem(section, name, value, scope);
  } else if (state == S_EXPECT_VALUE) {
    setItem(section, name, "", scope);
  }
}

bool Config::readFromFile(const std::string & filename, Scope scope)
{
  std::ifstream fin(filename.c_str());
  
  if (fin.fail()) {
    char buf[1024];
    snprintf(buf, 1024, "\nVarconf Error: could not open configuration file"
                         " \"%s\" for input.\n", filename.c_str());    
    sige.emit(buf);
    
    return false;
  }

  try {
    parseStream(fin, scope);
  }
  catch (ParseError p) {
    char buf[1024];
    std::string p_str = p;
    snprintf(buf, 1024, "\nVarconf Error: parsing exception thrown while "
                       "parsing \"%s\".\n%s", filename.c_str(), p_str.c_str());
    sige.emit(buf);
    return false;
  }
  
  return true;
}

void Config::setItem(const std::string & section,
                     const std::string & key,
                     const Variable & item,
                     Scope scope)
{
  if (key.empty()) {
    char buf[1024];
    snprintf(buf, 1024, "\nVarconf Warning: blank key under section \"%s\""
                         " sent to setItem() method.\n", section.c_str());
    sige.emit(buf);
  }
  else {
    std::string sec_clean = section; 
    std::string key_clean = key; 

    clean(sec_clean);
    clean(key_clean);

    item->setScope(scope);
    std::map<std::string, Variable> & section_map = m_conf[sec_clean];
    std::map<std::string, Variable>::const_iterator I = section_map.find(key_clean);
    if (I == section_map.end() || I->second != item) {
      section_map[key_clean] = item;
    }
 
    sig.emit(); 
    sigv.emit(sec_clean, key_clean);
    sigsv.emit(sec_clean, key_clean, *this);
  }
}

void Config::setParameterLookup(char s_name, const std::string & l_name, bool value)
{
    m_par_lookup[s_name] = std::pair<std::string, bool>(l_name, value);  
}

bool Config::writeToFile(const std::string & filename, Scope scope_mask) const
{
  std::ofstream fout(filename.c_str());

  if (fout.fail()) {
    char buf[1024];
    snprintf(buf, 1024, "\nVarconf Error: could not open configuration file"
                         " \"%s\" for output.\n", filename.c_str());
    sige.emit(buf);

    return false;
  }

  return writeToStream(fout, scope_mask);
}

bool Config::writeToStream(std::ostream & out, Scope scope_mask) const
{
  conf_map::const_iterator I;
  sec_map::const_iterator J;
 
  for (I = m_conf.begin(); I != m_conf.end(); I++) {
    out << std::endl << "[" << (*I).first << "]\n\n";

    for (J = (*I).second.begin(); J != (*I).second.end(); J++) {
      if (J->second->scope() & scope_mask) {
        out << (*J).first << " = \"" << (*J).second << "\"\n";
      }
    }
  }
  
  return true;
}

} // namespace varconf

