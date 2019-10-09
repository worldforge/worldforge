#include <iostream>

extern "C" {
#include "lauxlib.h"
}

int main() {
	luaL_newstate();
}