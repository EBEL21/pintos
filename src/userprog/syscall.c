#include "userprog/syscall.h"
//#include "../lib/user/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "threads/thread.h"
static void syscall_handler (struct intr_frame *);
static int32_t get_user(const uint8_t *uaddr);
//static bool put_user(uint8_t, *udst, uint8_t byte);
static int memread(void *src, void *dst, size_t size);

struct lock file_lock;

void
syscall_init (void) 
{
  lock_init(&file_lock);
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  //printf("syscall handler!!\n");
  int syscall_num = (int)*(uint32_t *)(f->esp);
  //printf("syscall num:%d\n",syscall_num);
  int byte_size = 0xc0000000 - (uintptr_t)(f->esp);
 // hex_dump(f->esp, f->esp, byte_size,true);
  void* argp = f->esp;
  switch(syscall_num) {
  case SYS_HALT:
	  halt();
	  NOT_REACHED();
	  break;
  case SYS_EXIT:	
	  check_addr(f->esp + 4);
	  int status = (int)*(uint32_t *)(f->esp + 4);
	  exit(status); 
	  NOT_REACHED();
	  break;
  case SYS_EXEC: {
	  check_addr(argp + 4);
	  void* cmd_line;
	  memread(f->esp +4, &cmd_line, sizeof(cmd_line));
          pid_t ret = exec((const char*)cmd_line);
	  f->eax = (uint32_t)ret;
	  break;
	}
  case SYS_WAIT:
	  {
	  pid_t pid;
	  memread(f->esp + 4,&pid,sizeof(pid_t));
	  int ret = wait(pid);
	  f->eax = (uint32_t)ret;
	  }
	  break;
  case SYS_CREATE: {
 	  const char* c_file = (const char*)*(uint32_t *)(f->esp + 4);
	  unsigned init_size = (unsigned)*(uint32_t *)(f->esp + 8);
	  f->eax = create(c_file, init_size);
	  } 
	  break;
  case SYS_REMOVE:
	  {
	  const char* r_file = (const char*)*(uint32_t *)(f->esp+4);
	  f->eax = remove(r_file);
	  }
	  break;
  case SYS_OPEN:
	  {
          const char* o_file = (const char*)*(uint32_t *)(f->esp+4);
          f->eax = open(o_file);
          }
	  break;
  case SYS_FILESIZE:
  	  {
          check_addr(f->esp + 4);
          int fd = (int)*(uint32_t *)(f->esp+4);
          f->eax = filesize(fd);
          }
	  break;
  case SYS_READ:
	  {
	  int fd = (int)*(uint32_t *)(argp + 4);
	  void* buffer = (void *)*(uint32_t *)(argp + 8);
	  unsigned size = (unsigned)*((uint32_t *)(argp + 12));
	  //int result;
	  f->eax = (uint32_t)read(fd, buffer, size);
	  //printf("read result: %d\n",result);
	  }
	  break;
  case SYS_WRITE:
	  {
	  int wfd = (int)*(uint32_t *)(f->esp + 4);
	  void* wbuffer = (void *)*(uint32_t *)(f->esp + 8);
	  unsigned wsize = (unsigned)*((uint32_t *)(f->esp + 12));
          //int wresult;
	  f->eax = (uint32_t)write(wfd, wbuffer, wsize);
	  //printf("write result: %d\n",wresult);
	  }
	  break;
  case SYS_SEEK:
	  {
          int fd = (int)*(uint32_t *)(f->esp+4);
          unsigned position = (unsigned)*(uint32_t*)(f->esp+8);
          seek(fd, position);
          }
	  break;
  case SYS_TELL:
	  {
          int fd = (int)*(uint32_t *)(f->esp+4);
          f->eax = tell(fd);
          }
	  break;
  case SYS_CLOSE:
	  {
	  int fd = (int)*(uint32_t *)(f->esp+4);
	  close(fd);
	  }
	  break;
  case SYS_FIBONACCI:
	  check_addr(f->esp + 4);
	  int n = (int)*(uint32_t *)(f->esp + 4);
	  f->eax = (uint32_t)fibonacci(n);
	  break;
  case SYS_SUM_FOUR:
	  check_addr(f->esp + 4);
	  check_addr(f->esp + 8);
	  check_addr(f->esp + 12);
	  check_addr(f->esp + 16);
	  int a = (int)*(uint32_t *)(f->esp + 4);
	  int b = (int)*(uint32_t *)(f->esp + 8);
	  int c = (int)*(uint32_t *)(f->esp + 12);
	  int d = (int)*(uint32_t *)(f->esp + 16);
	  f->eax = (uint32_t)sum_of_four_int(a, b, c, d);
	  break;
  default:
	break;
  }
}

void halt(void) {
	shutdown_power_off();
}
void exit(int status) {
	struct thread *cur = thread_current();
	int i = 0;
	cur->exit_status = status;
	printf("%s: exit(%d)\n", thread_name(), status);
	for(i = 3; i <128; i++) {
		if(cur->file[i] != NULL) {
			close(i);
		}
	}
	thread_exit();
}
pid_t exec(const char *cmd_line) {
	//printf("exec: %s\n",cmd_line);
	int pid =  process_execute(cmd_line);
        //sema_down(&(thread_current()->sema_load));
	//wait(pid);
	return pid;
	
}
int wait(pid_t pid) {
	//printf("wait: %d\n",pid);
	return process_wait(pid);
}
bool create(const char* file, unsigned initial_size)
{	
	if(file == NULL) {
		exit(-1);
	}
	check_addr(file);
	bool result;
	result = filesys_create(file, initial_size);
	return result;
}
bool remove(const char *file){
	bool result;
	check_addr(file);
	result = filesys_remove(file);
	return result;
}
int open(const char *file){
	struct thread* t = thread_current();
	if(file == NULL) {
		exit(-1);
	}
	check_addr(file);
	lock_acquire(&file_lock);
	struct file* fp = filesys_open(file);
	int i = 0;
	//lock_acquire(&file_lock);
	if(fp == NULL) {
		lock_release(&file_lock);
		return -1;
	}
	for(i = 3; i < 128; i++) {
		if(t->file[i] == NULL) {
			if(strcmp(thread_current()->name, file) ==0 ) {
				file_deny_write(fp);
			}
			t->file[i] = fp;
			lock_release(&file_lock);
			return i;
		}
	}
	lock_release(&file_lock);
	return -1;
	
}
int filesize(int fd){
	struct thread *t = thread_current();
	struct file* f = NULL;	
	if(fd < 3 || fd >= 128) return -1;
//	lock_acquire(&file_lock);
	f = t->file[fd];
	if(f == NULL) {
		exit(-1);
	}

	int size = (int)file_length(f);
//	lock_release(&file_lock);
	return size;
}

int read(int fd, void *buffer, unsigned size) {
	int i = 0;
	check_addr(buffer);
	struct thread *t = thread_current();
	lock_acquire(&file_lock);
	if (fd == 0) {
		for (i = 0; i < (int)size; i++) {
			((char *)buffer)[i] = input_getc();
			if (((char*)buffer)[i] == '\0') {
				break;
			}
		}
		i++;
	}else if(fd > 2) {
		if(t->file[fd] == NULL) {
			i = -1;
			exit(-1);
		} else {
		i=(int)file_read(t->file[fd], buffer, (off_t)size);
		}
	}else {
		i = -1;
	}
	lock_release(&file_lock);
	return i;
}

int write(int fd, const void *buffer, unsigned size) {
	int i =0;
	check_addr(buffer);
	struct thread* t = thread_current();
	lock_acquire(&file_lock);
	if (fd == 1) {
		putbuf(buffer,size);
		i = size;
	} else if(fd > 2) {
		if(t->file[fd] == NULL){
			i = -1;
			 exit(-1);
		} else {
		i=(int)file_write(t->file[fd], buffer, (off_t)size);
	}

	}else {
		i = -1;
	}
	lock_release(&file_lock);
	return i;
}

void seek(int fd, unsigned position){
	struct thread* t = thread_current();
	struct file *f;
	if(fd < 3 || fd >= 128) return -1;
//	lock_acquire(&file_lock);
	f = t->file[fd];
	if(f == NULL) {
		exit(-1);
	}
	file_seek(f, position);
//	lock_release(&file_lock);
}
unsigned tell(int fd){

	struct thread* t = thread_current();
	struct file *f;
	unsigned ret;
	if(fd < 3 || fd >= 128) return -1;
//	lock_acquire(&file_lock);
	f = t->file[fd];
	if(f == NULL) {
		exit(-1);
	}	
	ret = (unsigned)file_tell(f);
//	lock_release(&file_lock);
	return ret;
}
void close(int fd){
	struct thread *t = thread_current();
	struct file *f;
//	lock_acquire(&file_lock);i
	f = t->file[fd];
	if(f == NULL) {
//		lock_release(&file_lock);
		exit(-1);
	}
	file_close(f);
	t->file[fd] = NULL;
//	lock_release(&file_lock);
}

int fibonacci(int n) {
//	printf("fibonacci called!\n");
	int prev1 = 1, prev2 = 1;
	if (n < 0) {
		return -1;
	}
	else if (n == 0 || n == 1) {
		return 0;
	}
	else {
		int i = 2;
		do {
			int temp = prev1;
			prev1 = prev2;
			prev2 += temp;
			i++;
		} while (i != n);
	}
	return prev2;
}

int sum_of_four_int(int a, int b, int c, int d) {
	//printf("%d %d %d %d\n",a,b,c,d);
	return (a + b + c + d);
}

void check_addr(const void * addr)
{
	if (!is_user_vaddr(addr)) {
		//printf("not user addr!\n");
		exit(-1);
	}
}
static int
memread (void *src, void* dst, size_t size) {
	int32_t val;
	size_t i;
	for(i=0;i<size;i++) {
		val = get_user(src+i);
		if(val == -1) {
			exit(-1);
		}
		*(char*)(dst+i) = val & 0xff;
	}
	return (int)size;

}
static int32_t
get_user(const uint8_t *uaddr) {
 // check that a user pointer `uaddr` points below PHYS_BASE
  if (! ((void*)uaddr < PHYS_BASE)) {
    return -1;
  }

  // as suggested in the reference manual, see (3.1.5)
  int result;
  asm ("movl $1f, %0; movzbl %1, %0; 1:"
      : "=&a" (result) : "m" (*uaddr));
  return result;
}

static bool
put_user (uint8_t *udst, uint8_t byte) {
  // check that a user pointer `udst` points below PHYS_BASE
  if (! ((void*)udst < PHYS_BASE)) {
    return false;
  }

  int error_code;

  // as suggested in the reference manual, see (3.1.5)
  asm ("movl $1f, %0; movb %b2, %1; 1:"
      : "=&a" (error_code), "=m" (*udst) : "q" (byte));
  return error_code != -1;
}
