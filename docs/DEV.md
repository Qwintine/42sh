# 42sh — Developer Documentation

## Overview

42sh is structured as a modular shell implementation with a clear separation between parsing, expansion, and execution stages.

Execution flow:

input → lexer → parser → AST → expansion → execution

---

## Modules

### Lexer

Responsible for:

* Tokenizing input
* Recognizing operators and words

### Parser

Responsible for:

* Syntax analysis
* Building Abstract Syntax Tree (AST)

### Expansion

Responsible for:

* Environment variable expansion
* Word processing

### Executor

Responsible for:

* Forking processes
* Handling pipes
* Managing redirections
* Executing builtins and external commands

---

## Environment Management

Environment variables are stored in a hashmap for efficient lookup and modification.

---

## Memory Management

All allocated resources should be released after command execution:

* Free AST structures
* Close unused file descriptors
* Free temporary buffers

---

## Debugging Tips

Useful tools:

* gdb
* valgrind
* strace

Example:

```bash
valgrind ./src/42sh
```

---

## Possible Improvements

* Job control
* Full POSIX compatibility
* Extended quoting and expansion rules

