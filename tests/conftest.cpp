#include <varconf/varconf.h>

#include <sigc++/slot.h>

#include <iostream>
#include <string>

#include <cassert>

void callback( const std::string & section,
               const std::string & key,
               varconf::Config & conf)
{
  std::cout << "\nConfig Change: item " << key << " under section " << section
       << " has changed to " << conf.getItem( section, key) << ".\n"; 
}
 
void error( const char * message)
{
  std::cerr << message;
}

int main( int argc, char ** argv)
{
  varconf::Config config;

  config.sige.connect(sigc::ptr_fun(error));
  config.sigsv.connect(sigc::ptr_fun(callback));

  config.setParameterLookup( 'f', "foo", true);
  config.setParameterLookup( 'b', "bar", false);

  config.getCmdline( argc, argv);
  config.getEnv( "TEST_");
  assert(config.readFromFile( std::string(SRCDIR) + "/conf.cfg"));
  config.setItem( "tcp", "port", 6700, varconf::GLOBAL);
  config.setItem( "tcp", "v6port", 6700, varconf::USER);
  config.setItem( "console", "colours", "plenty", varconf::INSTANCE);
  config.setItem( "console", "speed", "fast", varconf::USER);

  assert(config.find("tcp", "port"));
  assert(config.find("console", "enabled"));
  assert(config.getItem("tcp", "port")->scope() == varconf::GLOBAL);
  //Default scope for read files are USER
  assert(config.getItem("console", "enabled")->scope() == varconf::USER);

  std::cout << "\nEnter sample configuration data to test parseStream() method.\n";

  try {
    config.parseStream( std::cin, varconf::USER);
  }
  catch ( varconf::ParseError p) {
    std::cout << "\nError while parsing from standard input stream.\n";
    std::cout << p;
  }

  config.writeToFile( "conf2.cfg");
  
  std::cout << "\nFile configuration data:\n"
            << "--------------------------\n"
            << config;

  std::cout << "\nUSER configuration data:\n"
            << "--------------------------\n";

  config.writeToStream(std::cout, varconf::USER);

  std::cout << "\nINSTANCE configuration data:\n"
            << "--------------------------\n";

  config.writeToStream(std::cout, varconf::INSTANCE);

  std::cout << "\nGLOBAL configuration data:\n"
            << "--------------------------\n";

  config.writeToStream(std::cout, varconf::GLOBAL);

  std::cout << "\nGLOBAL & USER configuration data:\n"
            << "--------------------------\n";

  config.writeToStream(std::cout, (varconf::Scope)(varconf::GLOBAL | varconf::USER));

  std::cout << "\nINSTANCE & USER configuration data:\n"
            << "--------------------------\n";

  config.writeToStream(std::cout, (varconf::Scope)(varconf::INSTANCE | varconf::USER));

  std::cout << "\nINSTANCE & GLOBAL configuration data:\n"
            << "--------------------------\n";

  config.writeToStream(std::cout, (varconf::Scope)(varconf::GLOBAL | varconf::INSTANCE));

  std::cout << "\nINSTANCE, USER & GLOBAL configuration data:\n"
            << "--------------------------\n";

  config.writeToStream(std::cout, (varconf::Scope)(varconf::GLOBAL | varconf::INSTANCE | varconf::USER));

  return 0;
}
