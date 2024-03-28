#  Basic File Manipulation with Bash Scripting

This project specifically targets files that begin with "CS3620" and appends the user's hawkid to each of these files.

## Objective

Write a Bash script that accomplishes the following tasks:

1. Recursively search through a given directory and its subdirectories to find files that begin with the string "CS3620".
2. Append the user's hawkid to the content of each identified file.

## Script Overview

The Bash script utilizes standard Unix commands like `find`, `grep`, and `echo` to iterate over the directory structure, identify the target files, and modify their content accordingly. 

## Usage

To run the script, ensure it has executable permissions and execute it from the command line, optionally passing the directory to search as an argument. If no directory is specified, the script should default to the current directory.

```bash
chmod +x script_name.sh
./script_name.sh [directory_path]
