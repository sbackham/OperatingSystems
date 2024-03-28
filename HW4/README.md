# Experimenting with Virtual Address Space of Linux Processes

This project shows the workings of Linux's virtual address space by providing practical utilities that interact with and manipulate the memory and file system, and show how Linux processes manage memory and files.

## Components

- **Memory Address Reader (`task1/task1.c`):** This program accepts a memory address and assesses its readability, echoing back the contents found at that address. It serves as a hands-on tool for exploring how processes interact with their virtual address space.
- **File Content Reverser (`task2/task2.c`):** Demonstrating file system interaction, this utility reverses the contents of a specified file, showcasing a straightforward example of file manipulation at the process level.

## Compilation

Scripts are provided for compiling the respective programs in their directories. To compile, navigate to the program's directory and run the corresponding script.

For `task1/task1.c`:
```bash
./task1/compile1.sh

For `task12/task2.c`:
```bash
./task2/compile2.sh
