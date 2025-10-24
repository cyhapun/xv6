// user/primes.c — xv6-riscv
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void primes(int left_fd) __attribute__((noreturn));

static int read_int(int fd, int *x) {
  int n = 0;
  unsigned char *p = (unsigned char *)x;
  while (n < sizeof(int)) {
    int r = read(fd, p + n, sizeof(int) - n);
    if (r == 0) return 0;
    if (r < 0) return 0;
    n += r;
  }
  return 1;
}

static void write_int(int fd, int x) {
  int n = 0;
  unsigned char *p = (unsigned char *)&x;
  while (n < sizeof(int)) {
    int w = write(fd, p + n, sizeof(int) - n);
    if (w <= 0) exit(1);
    n += w;
  }
}

void primes(int left_fd) {
  int prime;
  if (!read_int(left_fd, &prime)) {
    close(left_fd);
    exit(0);
  }
  printf("prime %d\n", prime);

  int next[2];
  if (pipe(next) < 0) {
    close(left_fd);
    exit(1);
  }

  int pid = fork();
  if (pid < 0) {
    close(left_fd);
    close(next[0]);
    close(next[1]);
    exit(1);
  }

  if (pid == 0) {
    close(next[1]);
    close(left_fd);
    primes(next[0]);
  } else {
    close(next[0]);
    int x;
    while (read_int(left_fd, &x)) {
      if (x % prime != 0) {
        write_int(next[1], x);
      }
    }
    close(left_fd);
    close(next[1]);
    wait(0);
    exit(0);
  }
}

int
main(int argc, char *argv[])
{
  int p[2];
  if (pipe(p) < 0) {
    fprintf(2, "primes: pipe failed\n");
    exit(1);
  }

  int pid = fork();
  if (pid < 0) {
    fprintf(2, "primes: fork failed\n");
    close(p[0]); close(p[1]);
    exit(1);
  }

  if (pid == 0) {
    close(p[1]);
    primes(p[0]);
  } else {
    close(p[0]);
    for (int i = 2; i <= 280; i++) {
      write_int(p[1], i);
    }
    close(p[1]);
    wait(0);
    exit(0);
  }
}
