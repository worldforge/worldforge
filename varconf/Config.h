#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>
#include <multimap.h>

#include "Variable.h"
#include "Observer.h"
#include "ParseError.h"

typedef void (conf_callback)(const string&, const string&);
typedef map< string, multimap<string, Observer*> > observer_map;
typedef map< string, multimap<string, conf_callback*> > callback_map;

/**
This class contains the actual configuration.
To access the Configuration, use Config::inst()
**/
class Config {
public:
  /// Call this to retrieve the single global configuration instance
  static Config* inst();

  Variable getItem(const string& section, const string& name);
  void setItem(const string& section, const string& name, const Variable
	       item);
  bool findItem(const string& section, const string& name);

  void registerObserver(Observer* observer, const string& section,
		        const string& name);
  void unregisterObserver(Observer* observer);

  void registerCallback(conf_callback* callback, const string& section,
		        const string& name);
  void unregisterCallback(conf_callback* callback, const string& section,
		          const string& name);

  bool readFromFile(const string& filename);

  bool writeToStream(ostream& ios);
  /// May throw a ParseError exception
  void parseStream(istream& ios);

  void getEnv(const string& prefix);
  void getCmdline(int argc, char** argv);

protected:
  Config();
  Config(const Config& config);
  Config& operator=(const Config&);
  
private:
  map< string, map<string, Variable> > m_conf;
  static Config* m_instance;

  observer_map m_observers;
  callback_map m_callbacks;
};
#endif
