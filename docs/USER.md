# 42sh — User Guide

## Running the Shell

Start interactive mode:

```bash
./src/42sh
```

Execute a single command:

```bash
./src/42sh -c "echo hello"
```

Execute a script file:

```bash
./src/42sh script.sh
```

---

## Supported Features

### Simple Commands

```bash
ls
echo hello
```

### Pipelines

```bash
ls | grep txt
```

### Redirections

```bash
echo hello > file.txt
cat < file.txt
```

### Environment Variables

```bash
VAR=value
echo $VAR
```

### Builtins

* cd
* exit
* export
* unset
* .

---

## Exit Status

The shell returns the exit status of the last executed command.

---

## Error Handling

Syntax errors and execution errors are printed to stderr.

---

## Limitations

This shell is an educational project and may not fully implement all POSIX features.

