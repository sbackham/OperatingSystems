#define _GNU_SOURCE //this is needed to be able to use execvpe 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>

typedef struct {
  char* binary_path;
  char* stdin;
  char* stdout;
  char* arguments;
  short wait;
} command;

// Function prototypes
void print_parsed_command(command);
short parse_command(command*, char*);
pid_t process_command(command, char* env[]);

// Read a line from a file
short getlinedup(FILE* fp, char** value) {
  char* line = NULL;
  size_t n = 0;
  int ret = getline(&line, &n, fp);
  if(ret == -1) {
    return 0;
  }
  line[strcspn(line, "\n")] = '\0';
  *value = strdup(line);
  free(line);
  return 1;
}

// Parse a command_file and set a corresponding command data structure
short parse_command(command* parsed_command, char* cmdfile) {
  FILE* fp = fopen(cmdfile, "r");
  if(!fp) {
    return 0;
  }
  char* value;
  short ret;
  int intvalue;

  ret = getlinedup(fp, &value);
  parsed_command->binary_path = value;

  ret = getlinedup(fp, &value);
  parsed_command->stdin = value;

  ret = getlinedup(fp, &value);
  parsed_command->stdout = value;

  ret = getlinedup(fp, &value);
  parsed_command->arguments = value;

  ret = getlinedup(fp, &value);
  intvalue = atoi(value);
  parsed_command->wait = (short)intvalue;

  fclose(fp);
  return 1;
}

// Useful for debugging
void print_parsed_command(command parsed_command) {
  printf("----------\n");
  printf("binary_path: %s\n", parsed_command.binary_path);
  printf("stdin: %s\n", parsed_command.stdin);
  printf("stdout: %s\n", parsed_command.stdout);
  printf("arguments: %s\n", parsed_command.arguments);
  printf("wait: %d\n", parsed_command.wait);
}

void free_command(command cmd) {
  free(cmd.binary_path);
  free(cmd.stdin);
  free(cmd.stdout);
  free(cmd.arguments);
}

pid_t process_command(command cmd, char* env[]) {
  pid_t pid = fork();
  if (pid == 0) {  // Child process
    if(cmd.stdout) {
      int fd = open(cmd.stdout, O_WRONLY|O_CREAT|O_TRUNC, 0664);
      dup2(fd, STDOUT_FILENO);
      close(fd);
    }
    if(cmd.stdin) {
      int fd = open(cmd.stdin, O_RDONLY);
      dup2(fd, STDIN_FILENO);
      close(fd);
    }
    char *args[] = {cmd.binary_path, cmd.arguments, NULL};
    execve(cmd.binary_path, args, env);
    perror("execve");
    exit(EXIT_FAILURE);
  } else if (pid > 0) {  // Parent process
    if (cmd.wait) {
      int status;
      waitpid(pid, &status, 0);
      printf("Child process %d terminated with exit code %d\n", pid, WEXITSTATUS(status));
    }
  } else {  // Fork failed
    perror("fork");
  }
  return pid;
}

void signal_handler(int signo, siginfo_t *info, void *context) {
  if (signo == SIGCHLD) {
    printf("Child process %d terminated with exit code %d\n", info->si_pid, info->si_status);
  }
}

static struct sigaction signal_action = {
  .sa_flags = SA_SIGINFO,
  .sa_sigaction = &signal_handler
};

int main(int argc, char *argv[], char* env[]) {
  pid_t *pids = malloc((argc - 1) * sizeof(pid_t));

  for(int ncommand=1; ncommand<argc; ncommand++) {
    command parsed_command;
    int ret = parse_command(&parsed_command, argv[ncommand]);
    if (!ret) {
      printf("command file %s is invalid\n", argv[ncommand]);
      continue;
    }
    print_parsed_command(parsed_command);
    pids[ncommand - 1] = process_command(parsed_command, env);
    free_command(parsed_command);
  }
  
  for (size_t i = 0; i < argc - 1; ++i) {
    waitpid(pids[i], NULL, 0);
  }

  free(pids);
  return 0;
}

