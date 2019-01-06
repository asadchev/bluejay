#ifndef BLUEJAY_CORE_FORWARD_H
#define BLUEJAY_CORE_FORWARD_H

#include <string>

namespace bluejay {

  struct system_exit {
    const int code = 0;
  };

  void initialize(int argc, char *argv[]);

  void finalize();

  void interactive();

  void exec_file(const std::string &file);

}

#endif /* BLUEJAY_CORE_FORWARD_H */
