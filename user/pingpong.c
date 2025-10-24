#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    int ping[2], pong[2]; // ping: cha->con, pong: con->cha
    if(pipe(ping) < 0 || pipe(pong) < 0){
        fprintf(2, "Pipe failed!\n");
        exit(1);
    }

    int pid = fork();
    if(pid < 0){
        fprintf(2, "Fork failed!\n");
        exit(1);
    }

    if(pid == 0){
        // Con
        char c;
        if(read(ping[0], &c, 1) != 1){
            fprintf(2, "Child read failed!\n");
            exit(1);
        }
        printf("%d: received ping\n", getpid());

        if(write(pong[1], &c, 1) != 1){
            fprintf(2, "Child write failed!\n");
            exit(1);
        }

        close(ping[0]); close(ping[1]);
        close(pong[0]); close(pong[1]);
        exit(0);
    } else {
        // Cha
        char c = 'x';
        if(write(ping[1], &c, 1) != 1){
            fprintf(2, "Parent write failed!\n");
            exit(1);
        }

        if(read(pong[0], &c, 1) != 1){
            fprintf(2, "Parent read failed!\n");
            exit(1);
        }
        printf("%d: received pong\n", getpid());

        close(ping[0]); close(ping[1]);
        close(pong[0]); close(pong[1]);

        wait(0);
        exit(0);
    }
}

