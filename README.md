# 42sh — POSIX-like Shell in C

A POSIX-like command interpreter written in C as part of the EPITA curriculum. <br>
This project focuses on systems programming concepts such as parsing, process management, file descriptor handling, and command execution.

---

## Features

### Execution

* Simple command execution
* Pipelines (`|`)
* Redirections (`>`, `<`, `>>`, file descriptor redirections)
* Execution of shell scripts (`.sh` files)

### Builtins

* `cd`
* `exit`
* `export`
* `unset`
* `.` (dot)

### Shell Semantics

* Environment variable storage and expansion
* Exit status propagation
* Error handling and diagnostics

### Engineering

* Modular architecture (lexer, parser, expansion, execution)
* Memory and resource cleanup
* Structured error handling

---

## Architecture Overview

The shell is structured in several stages:

input → lexer → parser → AST → expansion → execution

Modules:

* **Lexer**: tokenization of input
* **Parser**: syntax analysis and AST construction
* **Expansion**: environment variables and word expansion
* **Executor**: fork, exec, pipes and redirections
* **Builtins**: internal shell commands

---

## Technical Highlights

* Process management using `fork()`, `execve()`, and `waitpid()`
* Pipe and file descriptor handling using `dup2()` and `pipe()`
* Abstract Syntax Tree (AST) design for command execution
* Environment storage using a hashmap
* Careful memory and resource management

---

## Build

### Requirements

* GCC or Clang
* Make
* Autoconf / Automake

### Build Steps

```bash
autoreconf -fi
./configure
make
```

---

## Usage

Run interactive shell:

```bash
./src/42sh
```

Execute a single command:

```bash
./src/42sh -c "echo hello | wc -c"
```

Execute a script file:

```bash
./src/42sh script.sh
```

---

## Example

```bash
$ ./src/42sh script.sh
Hello from script
```

---

## Project Structure

src/ <br>
lexer/&nbsp;&nbsp; → &nbsp;&nbsp;Tokenization <br>
parser/&nbsp;&nbsp; → &nbsp;&nbsp;AST construction <br>
expand/&nbsp;&nbsp; → &nbsp;&nbsp;Variable expansion <br>
exec/&nbsp;&nbsp; → &nbsp;&nbsp;Execution engine <br>
builtins/&nbsp;&nbsp; → &nbsp;&nbsp;Builtin commands <br>
utils/&nbsp;&nbsp; → &nbsp;&nbsp;Utility functions <br>

tests/&nbsp;&nbsp; → &nbsp;&nbsp;Test suite <br>
docs/&nbsp;&nbsp; → &nbsp;&nbsp;Documentation <br>


---

## Documentation

* Developer notes: docs/DEV.md
* User guide: docs/USER.md
* API documentation: generated with Doxygen

---

## Limitations

This project is an educational shell and does not aim to be fully POSIX compliant. <br>
Some advanced features such as job control or full quoting rules may be incomplete.

---

## Authors

Quentin Pouyadoux - quentin.pouyadoux@epita.fr <br>
Victor Gardère - victor.gardere@epita.fr <br> 
Victor Happe - victor.happe@epita.fr <br>

