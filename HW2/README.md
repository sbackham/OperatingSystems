# Implementing a Shell with Linux Process API and Environment Handling

This project showcases the implementation of a basic shell using the Linux Process API and includes a utility to print command-line arguments and environment variables, demonstrating how a program interacts with its execution environment.

## Components

- **Shell Implementation (`shell.c`):** A C program that processes and executes commands based on configuration files, utilizing the Linux Process API.
- **Argument and Environment Printer (`print_args_envs.c`):** A utility that prints its command-line arguments and environment variables, useful for understanding how data is passed to programs in Unix/Linux.

## Compilation

The provided script `compile.sh` compiles both C programs:

```bash
#!/bin/bash

gcc -o print_args_envs print_args_envs.c
gcc -o shell shell.c