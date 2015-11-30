#include <ppapi_simple/ps_main.h>
#include <iostream>

int game_main(int argc, char* argv[]) {
  for (int i=0; i<argc; ++i) {
    std::cout << "Argument " << i << ": " << argv[i] << std::endl;
  }

  std::cerr << "Standard error output appears in the debug console\n";

  // Since PNaCl modules aren't supposed to terminate, we need to give
  // Pepper some breathing room to convey all the messages:
  sleep(3);

  return 0;
}

PPAPI_SIMPLE_REGISTER_MAIN(game_main)