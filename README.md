Quantum chemistry program/library I am writing for fun and to experiment with Python/C++/HPC/SWE.

General idea:
* Compute heavy pieces in C++/CUDA/etc
* Python interface via pybind11 (no traditional input file but rather a Python REPL or script)
* Extensible via via Python/C++, interfaces for integrals, arrays, etc
* Compatible with numpy arrays, python buffer protocol
* Support for duck-typed distributed arrays (GA implemented)
* Eistein summation/tensor contraction DSL
* Generation of many-body code on the fly given equations and machine constraints (kinda abstract JIT)


