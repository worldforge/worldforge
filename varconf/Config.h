#ifndef VARCONF_CONFIG_H
#define VARCONF_CONFIG_H

#include <string>
#include <map>
#include <multimap.h>

#include "Variable.h"
#include "Observer.h"
#include "ParseError.h"

namespace varconf {

typedef void (conf_callback)(const std::string&, const std::string&);
typedef map< std::string, multimap<std::string, Observer*> > observer_map;
typedef map< std::string, multimap<std::string, conf_callback*> > callback_map;
typedef map< char, pair<std::string, bool> > parameter_map;

/**
This class contains the actual configuration.
To access the Configuration, use Config::inst()
**/
class Config {
public:
  /// Call this to retrieve the single global configuration instance
  static Config* inst();

  Variable getItem(const std::string& section, const std::string& name);
  void setItem(const std::string& section, const std::string& name,
            const Variable item);
  bool findItem(const std::string& section, const std::string& name);

  void registerObserver(Observer* observer, const std::string& section,
		        const std::string& name);
  void unregisterObserver(Observer* observer);

  void registerCallback(conf_callback* callback, const std::string& section,
		        const std::string& name);
  void unregisterCallback(conf_callback* callback, const std::string& section,
		          const std::string& name);

  bool readFromFile(const std::string& filename);

  bool writeToFile(const std::string& filename);

  bool writeToStream(std::ostream& ios);
  /// May throw a ParseError exception
  void parseStream(std::istream& ios) throw (ParseError);

  void getEnv(const std::string& prefix);
  // reads-in all environment variables with the specified prefix 
  // and stores them in the 'unspecified' section (this prefix is currently
  // case-sensitive)

  void getCmdline(int argc, char** argv);

  void setParameterLookup(char shortForm, const std::string& longForm, bool needsValue);
  
protected:
  Config();
  Config(const Config& config);
  Config& operator=(const Config&);
  
private:
  std::map< std::string, std::map<std::string, Variable> > m_conf;
  static Config* m_instance;

  observer_map m_observers;
  callback_map m_callbacks;
  parameter_map m_parameter_lookup;
};

} // namespace varconf
#endif
