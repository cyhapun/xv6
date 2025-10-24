#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"
#include "kernel/fs.h" 

void xargs_main(int argc, char* argv[]) {
  char* new_argv[MAXARG];
  int base_argc = 0;

  if (argc < 2) {
    fprintf(2, "usage: xargs command [initial-args...]\n");
    exit(1);
  }

  int start_arg_index = 1;

  while (start_arg_index < argc && argv[start_arg_index][0] == '-') {
    if (strcmp(argv[start_arg_index], "-n") == 0) {
      start_arg_index += 2;
    }
    else {
      start_arg_index++;
    }

    if (start_arg_index >= argc) {
      fprintf(2, "xargs: command missing after options\n");
      exit(1);
    }
  }

  for (int i = start_arg_index; i < argc; i++) {
    if (base_argc >= MAXARG - 1) {
      fprintf(2, "xargs: too many initial arguments\n");
      exit(1);
    }
    new_argv[base_argc++] = argv[i];
  }

  if (base_argc == 0) {
    fprintf(2, "usage: xargs command [initial-args...]\n");
    exit(1);
  }

  char line_buf[512];
  int line_len = 0;
  char c;
  int n;

  while ((n = read(0, &c, 1)) > 0) {
    if (c == '\n') {
      goto process_line;

    }
    else if (line_len < sizeof(line_buf) - 1) {
      line_buf[line_len++] = c;

    }
    else {
      fprintf(2, "xargs: line too long, skipping\n");
      line_len = 0;
      while ((n = read(0, &c, 1)) > 0 && c != '\n');
      if (n == 0) break;
    }
    continue;

  process_line:
    if (line_len > 0) {
      line_buf[line_len] = '\0';

      int current_argc = base_argc;
      char* p = line_buf;

      while (*p == ' ' || *p == '\t') p++;

      while (*p != '\0') {
        char* word_start = p;
        while (*p != ' ' && *p != '\t' && *p != '\0') p++;

        if (word_start != p) {
          if (current_argc >= MAXARG - 1) {
            fprintf(2, "xargs: too many arguments from input\n");
            break;
          }

          if (*p != '\0') {
            *p = '\0';
            p++;
          }

          new_argv[current_argc++] = word_start;
        }

        while (*p == ' ' || *p == '\t') p++;
      }

      new_argv[current_argc] = 0;

      int pid = fork();
      if (pid == 0) {
        exec(new_argv[0], new_argv);
        fprintf(2, "xargs: exec failed for %s\n", new_argv[0]);
        exit(1);
      }
      else if (pid < 0) {
        fprintf(2, "xargs: fork failed\n");
        exit(1);
      }
      else {
        wait(0);
      }
    }

    line_len = 0;
  }

  if (line_len > 0) {
    line_buf[line_len] = '\0';

    int current_argc = base_argc;
    char* p = line_buf;

    while (*p == ' ' || *p == '\t') p++;

    while (*p != '\0') {
      char* word_start = p;
      while (*p != ' ' && *p != '\t' && *p != '\0') p++;

      if (word_start != p) {
        if (current_argc >= MAXARG - 1) {
          fprintf(2, "xargs: too many arguments from input on EOF\n");
          break;
        }

        if (*p != '\0') {
          *p = '\0';
          p++;
        }

        new_argv[current_argc++] = word_start;
      }
      while (*p == ' ' || *p == '\t') p++;
    }

    new_argv[current_argc] = 0;

    int pid = fork();
    if (pid == 0) {
      exec(new_argv[0], new_argv);
      fprintf(2, "xargs: exec failed for %s\n", new_argv[0]);
      exit(1);
    }
    else if (pid > 0) {
      wait(0);
    }
  }

  exit(0);
}


int main(int argc, char* argv[]) {
  if (argc < 2) {
    fprintf(2, "usage: xargs command [initial-args...]\n");
    exit(1);
  }
  xargs_main(argc, argv);
  return 0;
}
