#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h" 
#include "kernel/fs.h" 

void xargs_main(int argc, char *argv[]) {
    char *new_argv[MAXARG];
    int base_argc = 0;
    
    if (argc < 2) {
        fprintf(2, "usage: xargs command [initial-args...]\n");
        exit(1);
    }
    
    int start_arg_index = 1;
    
    while (start_arg_index < argc && argv[start_arg_index][0] == '-') {
        
        if (strcmp(argv[start_arg_index], "-n") == 0) {
            start_arg_index += 2;
        } else {
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
        if (line_len < sizeof(line_buf) - 1) {
            line_buf[line_len++] = c;    
        } else {
            fprintf(2, "xargs: line too long, skipping\n");
            line_len = 0;
            while ((n = read(0, &c, 1)) > 0 && c != '\n');
            if (n == 0) break;
        }
        continue;    

        exit(0);
    }
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(2, "usage: xargs command [initial-args...]\n");
        exit(1);
    }
    xargs_main(argc, argv);
    return 0;
}