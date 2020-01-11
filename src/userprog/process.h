#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"
#include "syscall.h":

//#define PID_ERROR ((pid_t) -1)
//#define PID_INITAILIZING ((pid_t)-2)

tid_t process_execute (const char * file_name);
int process_wait (tid_t tid);
void process_exit (void);
void process_activate (void);
struct thread* get_child(pid_t pid);
void remove_child_process(struct thread*);
#endif /* userprog/process.h */
