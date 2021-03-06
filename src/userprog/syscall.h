#include "../lib/user/syscall.h"
#include "../threads/vaddr.h"
#include "userprog/process.h"
#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H


void syscall_init (void);
void halt(void);
void exit(int status);
pid_t exec(const char *cmd_line);
int wait(pid_t pid);
bool create(const char* file, unsigned initial_size);
bool remove(const char *file);
int open(const char *file);
int filesize(int fd);
int read(int fd, void *buffer, unsigned size);
int write(int fd, const void *buffer, unsigned size);
void seek(int fd, unsigned position);
unsigned tell(int fd);
void close(int fd);
int fibonacci(int n);
int sum_of_four_int(int a, int b, int c, int d);

void check_addr(const void* addr);

#endif /* userprog/syscall.h */
