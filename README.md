# 42sh — POSIX-like Shell in C

Implementation of a POSIX-like shell developed as part of the EPITA curriculum.  
The project focuses on parsing, expansion and execution of shell commands with an architecture inspired by real shells.

## Features

- Execution of simple commands
- Pipelines (`|`)
- Redirections (`>`, `<`, `>>`, file descriptors)
- Builtins (cd, exit, export, unset, dot, …)
- Environment variables and expansion
- Command parsing into an AST
- Error handling and exit status management

## Build

Requirements:
- gcc or clang
- make
- autoconf / automake (if building from configure.ac)

Build:

```bash
autoreconf -fi
./configure
make

