#include <unistd.h>
#include "Config.h"

void callback(const string& section, const string& name)
{
  cout << section << "->" << name << " has changed to "
       << Config::Instance()->getItem(section, name) << "!" << endl;
}

int main(int argc, char** argv)
{
  if (argc < 2) {
    if (isatty(STDIN_FILENO)) cout << "Go ahead and type your own "
                                      "configuration" << endl;
    try {
      Config::Instance()->parseStream(cin);
    }
    catch (ParseError p)
    {
      cout << "While parsing from standard input:\n";
      cout << p;
    }
  } else {
    Config::Instance()->readFromFile(argv[1]);
  }
  Config::Instance()->writeToStream(cout);
  cout << "---" << endl;
  Config::Instance()->registerCallback(&callback, "tcp", "port");
  Config::Instance()->registerCallback(&callback, "console", "colours");
  Config::Instance()->setItem("tcp", "port", 6700);
  Config::Instance()->setItem("console", "colours", "plenty");
}
