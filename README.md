# Mini Shell

![C](https://img.shields.io/badge/C-Language-blue)
![Linux](https://img.shields.io/badge/UNIX-Compatible-green)
![Operating Systems](https://img.shields.io/badge/OS-Concepts-orange)
![Shell](https://img.shields.io/badge/Shell-Interpreter-purple)

**Mini Shell** is a minimalist UNIX-style shell implemented in C for learning and demonstrating **systems programming** and **operating system concepts** such as process control, file I/O, signal handling, and job management.

---

## Features

- Built-in commands: `exit`, `cd`, `pwd`, `echo`, `alias`, `unalias`, `history`
- Input/output redirection: `>`, `>>`, `<`
- Command chaining using pipes `|`
- Maintains command history
- Shell-style expansion: wildcard (`*`) and pattern matching
- Parses arguments, supports quoting and multiple commands per line

---

## Example Commands

```bash
pwd
cd /home/user
echo "Hello, world!" > hello.txt
cat < hello.txt | grep Hello >> log.txt
ls *.c | wc -l
alias ll="ls -la"
