/*
 *  Config.h - interface for main configuration class
 *
 *  Copyright (C) 2000, The WorldForge Project.
 */
 
#ifndef VARCONF_CONFIG_H
#define VARCONF_CONFIG_H

#include <string>
#include <map>
#include <multimap.h>

#include <sigc++/signal_system.h>
#include "Variable.h"
#include "ParseError.h"

namespace varconf {

typedef std::map< std::string, std::map< std::string, Variable > > conf_map;
typedef std::map< std::string, Variable> sec_map;
typedef std::map< char, pair<std::string, bool> > parameter_map;

class Config {
public:
  static Config* inst();
  // returns a pointer to the global configuration instance and 
  // initializes it if it does not already exist.
 
  bool erase( const std::string& section, const std::string& key = ""); 

  bool find( const std::string& section, const std::string& key = "");

  void clean( std::string& str);
 
  Variable getItem( const std::string& section, const std::string& key);

  void setItem( const std::string& section, const std::string& key, 
                const Variable item);

  bool findItem( const std::string& section, const std::string& key)
  {
    return find( section, key);
  } 

  bool readFromFile( const std::string& filename);
  // Precondition: valid filename of an existing configuration file.
  // Postcondition: configuration data in the passed configuration file is
  //                parsed and stored; 'false' is returned if the file cannot
  //                be opened; any parsing errors are reported to 'cerr'. 

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

  void parseStream( std::istream& in) throw ( ParseError);
  // Precondition: valid input stream to read configuration data from.
  // Postcondition: configuration data is read from the input stream
  //                and stored in memory; a 'ParseError' object is thrown
  //                when parsing errors are encountered.

  void getEnv( const std::string& prefix);
  // reads-in all environment variables with the specified prefix
  // (prefix is currently case-sensitive)

  void getCmdline( int argc, char** argv);
  // reads-in all commandline arguments according to the format in
  // config_format and stores them in the m_conf config variable.

  void setParameterLookup( char s_name, const std::string& l_name, 
                           bool value = false);
  // attachs a one letter commandline argument (ie: -a) to a longe_name
  // version and specifies whether it is followed by a value or not
  // (ie: "-a 45" vs. "-a")

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
