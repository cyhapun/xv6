#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int ping[2], pong[2]; // ping: cha -> con, pong: con -> cha
    
    // Tạo 2 pipe để truyền dữ liệu 2 chiều giữa cha và con
    if(pipe(ping) < 0 || pipe(pong) < 0){
        fprintf(2, "Pipe failed!\n");
        exit(1);
    }

    int pid = fork(); // Tạo tiến trình con
    if(pid < 0){
        fprintf(2, "Fork failed!\n");
        exit(1);
    }

    if(pid == 0){
        // Tiến trình con
        close(ping[1]);  // không ghi vào ping
        close(pong[0]);  // không đọc từ pong

        char c;
        // Nhận 1 byte dữ liệu từ cha qua pipe ping
        if(read(ping[0], &c, 1) != 1){
            fprintf(2, "Child read failed!\n");
            exit(1);
        }
        printf("%d: received ping\n", getpid());
        
        // Gửi lại ký tự đó cho cha qua pipe pong
        if(write(pong[1], &c, 1) != 1){
            fprintf(2, "Child write failed!\n");
            exit(1);
        }

        close(ping[0]);
        close(pong[1]);
        exit(0);
    } else {
        // Tiến trình cha
        close(ping[0]);  // không đọc từ ping
        close(pong[1]);  // không ghi vào pong

        char c = 'x';
        // Gửi 1 byte sang tiến trình con
        if(write(ping[1], &c, 1) != 1){
            fprintf(2, "Parent write failed!\n");
            exit(1);
        }
        
        // Nhận phản hồi từ con
        if(read(pong[0], &c, 1) != 1){
            fprintf(2, "Parent read failed!\n");
            exit(1);
        }
        printf("%d: received pong\n", getpid());

        close(ping[1]);
        close(pong[0]);

        wait(0);
        exit(0);
    }
}

