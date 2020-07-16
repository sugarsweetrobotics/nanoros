#include "nanoros/os.h"
#include "nanoros/rosmaster.h"


int list_topics(const int argc, const char* argv[]) {
  
  return 0;
}

int main(const int argc, const char* argv[]) {

  if (argc > 2) {
    std::string cmd = argv[1];

    if (cmd == "list") {
      return list_topics(argc, argv);
    }
    
  }
  

  return 0;
}
