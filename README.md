# 42sh — POSIX-like Shell in C

Implementation of a POSIX-like shell developed at EPITA.

This project implements a command interpreter capable of parsing, expanding and executing shell commands using an architecture inspired by real Unix shells.  
The focus of the project is on **systems programming, process management, parsing, and file descriptor handling**.

---

## Key Features

- Execution of simple commands
- Pipelines (`|`)
- Redirections (`>`, `<`, `>>`, file descriptors`)
- Builtins (`cd`, `exit`, `export`, `unset`, `.`)
- Environment variables and expansion
- Parsing into an Abstract Syntax Tree (AST)
- Error handling and exit status propagation
- Memory management and resource cleanup

---

## Architecture Overview

The shell is structured in several stages:

input → lexer → parser → AST → expansion → execution

### Modules:

- **Lexer**: tokenization of input
- **Parser**: syntax analysis and AST construction
- **Expansion**: environment variables and word expansion
- **Executor**: fork, exec, pipes and redirections
- **Builtins**: internal shell commands

---

## Build

### Requirements
- GCC or Clang
- Make
- Autoconf / Automake

### Build

```bash
autoreconf -fi
./configure
make

## Usage
Run interactive shell:

./src/42sh
Execute a single command:

./src/42sh -c "echo hello | wc -c"
### Example

$ ./src/42sh
42sh$ echo hello | wc -c

## Project Structure
src/
  lexer/       Tokenization
  parser/      AST construction
  expand/      Variable expansion
  exec/        Execution engine
  builtins/    Builtin commands
  utils/       Utility functions

## Documentation
Developer notes: docs/DEV.md

User guide: docs/USER.md

API documentation: generated with Doxygen

## Limitations
This project is an educational shell and does not aim to be fully POSIX compliant.
Some advanced features such as job control or full quoting rules may be incomplete.

## Authors
Quentin Pouyadoux
Victor Gardère
Victor Happe
EPITA — Systems Programming 

