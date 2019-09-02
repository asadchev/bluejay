#include "bluejay/core/forward.h"
#include "bluejay/ga/forward.h"

#define OMPI_SKIP_MPICXX
#include <mpi/mpi.h>

#include <iostream>

int main(int argc, char *argv[]) {
  MPI_Init(0, NULL);
  bluejay::initialize(argc, argv);
  bluejay::ga::initialize();
  int code = 0;
  try {
    if (argc == 1) {
      bluejay::interactive();
    }
    else {
      try {
        bluejay::exec_file(argv[1]);
      }
      catch (const std::exception &e) {
        std::cerr << e.what();
        code = 1;
      }
    }
  }
  catch (const bluejay::system_exit &e) {
    code = e.code;
  }
  bluejay::finalize();
  bluejay::ga::finalize();
  MPI_Finalize();
  return code;
}
