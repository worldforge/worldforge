#include <string>
#include <iostream>
#include <fstream>
#include "Config.h"

extern char **environ;

using namespace std;

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
    if (((c >= 'a') && (c <= 'z') || (c >= 'A') && (c <= 'Z'))) return C_ALPHA;
    if (isdigit(c)) return C_NUMERIC;
    if ((c == '-') || (c == '_')) return C_DASH;
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

Config* Config::m_instance;

Config::Config()
{
}

Config* Config::inst()
{
  if (m_instance == NULL) m_instance = new Config;
  return m_instance;
}

Variable Config::getItem(const string& section, const string& name)
{
  return (m_conf[section])[name];
}

void Config::setItem(const string& section, const string& name,
                     const Variable item)
{
  string sec = section, nam = name;
  for (size_t i = 0; i < sec.size(); i++) {
    ctype_t c = ctype(sec[i]);
    if ((c != C_NUMERIC) && (c != C_ALPHA) && (c != C_DASH)) sec[i] = '_';
    sec[i] = tolower(sec[i]);
  }
  for (size_t i = 0; i < nam.size(); i++) {
    ctype_t c = ctype(nam[i]);
    if ((c != C_NUMERIC) && (c != C_ALPHA) && (c != C_DASH)) nam[i] = '_';
    nam[i] = tolower(nam[i]);
  }
  (m_conf[sec])[nam] = item;

  observer_map::iterator I;

  if ((I = m_observers.find(section)) != m_observers.end()) {
    for (multimap<string, Observer*>::iterator J = (*I).second.begin();
         J != (*I).second.end(); J++) {
      if ((*J).first == name) (*J).second->update();
    }
  }
  callback_map::iterator C;

  if ((C = m_callbacks.find(section)) != m_callbacks.end()) {
    for (multimap<string, conf_callback*>::iterator J = (*C).second.begin();
         J != (*C).second.end(); J++) {
      if ((*J).first == name) (*(*J).second)(section, name);
    }
  }

}

bool Config::findItem(const string& section, const string& name)
{
  return ((m_conf.count(section)) && (m_conf[section].count(name)));
}

void Config::registerObserver(Observer* observer, const string& section,
                              const string& name)
{
    m_observers[section].insert(pair<string, Observer*>(name, observer));
}

void Config::unregisterObserver(Observer* observer)
{
    for (map< string, multimap<string, Observer*> >::iterator I =
         m_observers.begin(); I != m_observers.end(); I++) {
        for (multimap<string, Observer*>::iterator J = (*I).second.begin();
             J != (*I).second.end(); J++) {
                 if ((*J).second == observer) {
                     m_observers.erase(I);
                     break;
                 }
        }
    }
}

void Config::registerCallback(conf_callback* callback, const string& section,
                              const string& name)
{
    m_callbacks[section].insert(pair<string, conf_callback*>(name,callback));
}

void Config::unregisterCallback(conf_callback* callback, const string& section,
                                const string& name)
{
    for (callback_map::iterator I = m_callbacks.begin();
         I != m_callbacks.end(); I++) {
        for (multimap<string, conf_callback*>::iterator J = (*I).second.begin();
             J != (*I).second.end(); J++) {
                 if ((*J).second == callback) {
                     m_callbacks.erase(I);
                     break;
                 }
        }
    }
}

bool Config::readFromFile(const string& filename)
{
  ifstream in(filename.c_str());
  if (!in) {
    cerr << "Could not open " << filename << " for input!\n";
    return false;
  }
  try {
    parseStream(in);
  }
  catch (ParseError p) {
    cerr << "While parsing " << filename << ":\n";
    cerr << p;
  }
  return true;
}

bool Config::writeToFile(const string& filename)
{
    ofstream out(filename.c_str());
    if (!out) {
      cerr << "Could not open " << filename << " for output!\n";
      return false;
    }
    return writeToStream(out);
}

bool Config::writeToStream(ostream& ios)
{
  map< string, map<string, Variable> >::iterator I;
  map<string, Variable>::iterator J;
  for (I = m_conf.begin(); I != m_conf.end(); I++) {
    ios << endl << "[" << (*I).first << "]\n\n";
    for (J = (*I).second.begin(); J != (*I).second.end(); J++) {
      ios << (*J).first << " = \"" << (*J).second
          << "\"\n";
    }
  }
  return true;
}

void Config::parseStream(istream& ios) throw (ParseError)
{
  char c; bool escaped = false;
  size_t line = 1, col = 0;
  string name = "", value = "", section = "";
  state_t state = S_EXPECT_NAME;
  while (ios.get(c)) {
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
	    throw ParseError("item name", line, col);
	    break;
	}
	break;
      case S_SECTION :
	switch (ctype(c)) {
	  case C_ALPHA:
	  case C_NUMERIC:
	    section += c;
	    break;
	  case C_SQUARE_CLOSE:
	    state = S_EXPECT_EOL;
	    break;
	  default:
	    throw ParseError("']'", line, col);
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
	    throw ParseError("'='", line, col);
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
	    throw ParseError("'='", line, col);
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
	  case C_SPACE:
	    break;
	  default:
	    throw ParseError("value", line, col);
	    break;
	}
	break;
      case S_VALUE:
	switch (ctype(c)) {
	  case C_QUOTE:
	    throw ParseError("value", line, col);
	  case C_SPACE:
	    state = S_EXPECT_EOL;
	    setItem(section, name, value);
	    break;
	  case C_EOL:
	    state = S_EXPECT_NAME;
	    setItem(section, name, value);
	    break;
	  case C_HASH:
	    state = S_COMMENT;
	    setItem(section, name, value);
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
	      setItem(section, name, value);
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
	    throw ParseError("end of line", line, col);
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
  }
  if (state == S_QUOTED_VALUE) throw ParseError("\"", line, col);
  if (state == S_VALUE) setItem(section, name, value);
}

void Config::getCmdline(int argc, char** argv)
{
  // this is stolen directly from the thor.cc in scratchpad
  // We put commandline args into the 'nonspecified' section
  // it is searched after the 'specified' sections 
  string name, value, section="";
  bool valueNext = false;
  int i, j, a=0;
 
  // preprocess the args.  Switch short forms for long forms.  If we run
  // across some short form that we don't know about, skip it and raise a
  // warning
  for (i = 1; i < argc; i++) {
    string arg(argv[i]);
    if (argv[i][0] == '-') {
      if (argv[i][1] =='-') {
        // arg begins with "--"
        size_t eqPos = arg.find('=');
        if (eqPos != string::npos) {
          // we know where the = is
          value = arg.substr(eqPos + 1, arg.size() - (eqPos + 1));    
        }
        else {
          // string with no =
          value = "";
        }
        name = arg.substr(2, eqPos - 2);
      }
      else /* if (argv[i][2] == '\n')*/ {
        char shortName = argv[i][1];
        // This is a single-char config.
        parameter_map::iterator I;
        
        if((I = m_parameter_lookup.find(shortName)) != m_parameter_lookup.end()) {
          // this parameter exists in our lookup table
          name = ((*I).second).first;
          valueNext = ((*I).second).second;
          // clear the value string if valueNext is false
          if (!valueNext) {
            value = "";
          }
        }
        else {
          // This gets executed if we can't match a long name to a short one
          // I think it's better to just keep going, and ignore the bad stuff
          // for now- later on we should raise a warning
          valueNext = false;
        }
      }
    }
    else if ((valueNext) && (argv[i][0] != '\0')) {
      // If we're expecting a value and it is not null, grab it
      value = argv[i];
      valueNext = false;
    }    
    
    if(!valueNext) {
      // insert when we've got all the information
      setItem(section, name, value);
    }
  }

}

void Config::getEnv(const string& prefix)
{
  /*
  string name, value, section="";

  int j, a = 0;
  for ( int i = 0; environ[i] != NULL; i++)
    if ((environ[i][0]=='W')&&(environ[i][1]=='F')&&(environ[i][2]=='_')) {
      string env( environ[i]); 
      name = value = "";
     
      size_t eq_pos = env.find( '='); 
      if ( eq_pos != string::npos) {
        value = env.substr( ( eq_pos + 1), ( env.size() - ( eq_pos + 1)));
      }
      else {
        value = ""; // no value if '=' not found
      }
      name = env.substr( 3, eq_pos - 3);

      if ( name.size() )
        setItem( section, name, value);
      else
        throw "Invalid environment setting!";
    }*/
}

void Config::setParameterLookup(char shortForm,
                                const string& longForm,
                                bool needsValue = true) {
    m_parameter_lookup[shortForm] = pair<string, bool>(longForm, needsValue);  
        
}

} // namespace varconf
