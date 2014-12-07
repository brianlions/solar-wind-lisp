## Introduction

solar-wind-lisp is a small LISP interpreter written in C++.

## Build and Install

Use *make* to build, the exetuable will be placed in the *bin/* directory.

### Dependency

solar-wind-lisp depends on [Boost library](http://www.boost.org) and [Google
gtest](http://code.google.com/p/googletest), so you may need to install these
two libraries and update *include_paths* and *library_paths* in the Makefile
before executing the *make* command.

To run all the unit test cases:

    make runtest

Optionally if you have already installed the [Valgrind](http://valgrind.org) dynamic
analysis tool, use this command to run the memory detector:

    make valgrindtest

## Usage

There are several sample scripts in the *samples* directory, read the comments
in those scripts to get started.

To start the REPL:

    ./bin/solarwind-repl

To execute a script:

    ./bin/solarwind-repl ./samples/01-basic.swl
