#include "bluejay/core/forward.h"
#include "bluejay/core/system.h"
#include "bluejay/core/pybind11.h"

#include <pybind11/embed.h>
#include <pybind11/pytypes.h>
#include <Python.h>

#include <iostream>
#include <cstdlib>

void bluejay::initialize(int argc, char *argv[]) {
  auto pythonpath = system::libpath() / "python" ;
  // Py_SetProgramName(argv[0]);
  py::initialize_interpreter();
  {
    wchar_t *wargv[] = { };
    PySys_SetArgv(0, wargv);
  }
  py::module::import("sys").attr("path").attr("insert")(0, pythonpath.string());
  // load core python bindings
  py::module::import("bluejay_core");
}

void bluejay::finalize() {
  py::finalize_interpreter();
}

void bluejay::interactive() {
  char *startup = Py_GETENV("PYTHONSTARTUP");
  if (startup != NULL && startup[0] != '\0') {
    py::eval_file(startup);
  }
  PyRun_InteractiveLoop(stdin, "<stdin>");
}

void bluejay::exec_file(const std::string &file) {
  py::object scope = py::module::import("__main__").attr("__dict__");
  scope["__file__"] = file;
  try {
    py::eval_file(file, scope);
  }
  catch (const pybind11::error_already_set &e) {
    PyDict_DelItemString(scope.ptr(), "__file__");
    if (e.matches(PyExc_SystemExit)) {
      throw system_exit{ py::cast<int>(e.value.attr("code")) };
    }
    throw e;
  }
  PyDict_DelItemString(scope.ptr(), "__file__");
}
