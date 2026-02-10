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
<pre>
lexer/      Tokenization <br>
parser/      AST construction <br>
expand/      Variable expansion <br>
exec/        Execution engine <br>
builtins/    Builtin commands <br>
utils/       Utility functions <br>

tests/         Test suite <br>
docs/ Documentation <br>
</pre>

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

Quentin Pouyadoux <br>
Victor Gardère <br>
Victor Happe <br>

