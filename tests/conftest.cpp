#include <varconf/varconf.h>

#include <iostream>
#include <string>

using namespace varconf;
using namespace SigC;

void callback( const std::string& section, const std::string& key, Config& conf)
{
  std::cout << "\nConfig Change: item " << key << " under section " << section
       << " has changed to " << conf.getItem( section, key) << ".\n"; 
}
 
void error( const char* message)
{
  std::cerr << message;
}

int main( int argc, char** argv)
{
  Config config;

  config.sige.connect( slot( error));
  config.sigsv.connect( slot( callback));

  config.setParameterLookup( 'f', "foo", true);
  config.setParameterLookup( 'b', "bar", false);

  config.getCmdline( argc, argv);
  config.getEnv( "TEST_");
  config.readFromFile( "conf.cfg");
  config.setItem( "tcp", "port", 6700);
  config.setItem( "console", "colours", "plenty");

  std::cout << "\nEnter sample configuration data to test parseStream() method.\n";

  try {
    config.parseStream( std::cin);
  }
  catch ( ParseError p) {
    std::cout << "\nError while parsing from standard input stream.\n";
    std::cout << p;
  }

  config.writeToFile( "conf2.cfg");
  
  std::cout << "\nFile configuration data:\n"
            << "--------------------------\n"
            << config;

  return 0;
}
