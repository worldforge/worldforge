#include <unistd.h>
#include "Config.h"

using namespace varconf;

void callback(const string& section, const string& name)
{
  cout << section << "->" << name << " has changed to "
       << Config::inst()->getItem(section, name) << "!" << endl;
}

int main(int argc, char** argv)
{
  Config::inst()->setParameterLookup('f', "foo", true);
  Config::inst()->setParameterLookup('b', "bar", false);
  
  if (argc < 2) {
    if (isatty(STDIN_FILENO)) cout << "Go ahead and type your own "
                                      "configuration" << endl;
    try {
      Config::inst()->parseStream(cin);
    }
    catch (ParseError p)
    {
      cout << "While parsing from standard input:\n";
      cout << p;
    }
  } else {
    Config::inst()->getCmdline(argc, argv);
  }
  Config::inst()->writeToStream(cout);
  cout << "---" << endl;
  Config::inst()->registerCallback(&callback, "tcp", "port");
  Config::inst()->registerCallback(&callback, "console", "colours");
  Config::inst()->setItem("tcp", "port", 6700);
  Config::inst()->setItem("console", "colours", "plenty");
}
