/*
 * This file is part of the becquerel.org Curie project.
 * See the appropriate repository at http://git.becquerel.org/ for exact file
 * modification records.
*/

/*
 * Copyright (c) 2008-2011, Kyuba Project Members
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
*/

/*! \file
 *  \brief Syscall Header (Linux)
 */

#ifndef LIBSYSCALL_SYSCALL_H
#define LIBSYSCALL_SYSCALL_H

#define linux_syscalls

#include <syscall/invocation.h>
#include <asm/unistd.h>

#ifndef define_syscall0
#define define_syscall0(num,id,rid,rettype)\
rettype rid ();
#endif

#ifndef define_syscall1
#define define_syscall1(num,id,rid,rettype,p1type)\
rettype rid (p1type a1);
#endif

#ifndef define_syscall2
#define define_syscall2(num,id,rid,rettype,p1type,p2type)\
rettype rid (p1type a1, p2type a2);
#endif

#ifndef define_syscall3
#define define_syscall3(num,id,rid,rettype,p1type,p2type,p3type)\
rettype rid (p1type a1, p2type a2, p3type a3);
#endif

#ifndef define_syscall4
#define define_syscall4(num,id,rid,rettype,p1type,p2type,p3type,p4type)\
rettype rid (p1type a1, p2type a2, p3type a3, p4type a4);
#endif

#ifndef define_syscall5
#define define_syscall5(num,id,rid,rettype,p1type,p2type,p3type,p4type,p5type)\
rettype rid (p1type a1, p2type a2, p3type a3, p4type a4, p5type a5);
#endif

#ifndef define_syscall6
#define define_syscall6(num,id,rid,rettype,p1type,p2type,p3type,p4type,p5type,p6type)\
rettype rid (p1type a1, p2type a2, p3type a3, p4type a4, p5type a5, p6type a6);
#endif

#ifdef __NR_read
#define have_sys_read
define_syscall3 (__NR_read, read, sys_read, signed int, unsigned int, void *, unsigned int)
#endif
#ifdef __NR_write
#define have_sys_write
define_syscall3 (__NR_write, write, sys_write, signed int, unsigned int, void *, unsigned int)
#endif
#ifdef __NR_open
#define have_sys_open
define_syscall3 (__NR_open, open, sys_open, long, const char *, int, int)
#endif
#ifdef __NR_close
#define have_sys_close
define_syscall1 (__NR_close, close, sys_close, long, unsigned int)
#endif
#ifdef __NR_stat
#define have_sys_stat
define_syscall2 (__NR_stat, stat, sys_stat, long, char *, void *)
#endif
#ifdef __NR_newstat
#define have_sys_newstat
define_syscall2 (__NR_newstat, newstat, sys_newstat, long, char *, void *)
#endif
#ifdef __NR_fstat
#define have_sys_fstat
define_syscall2 (__NR_fstat, fstat, sys_fstat, long, unsigned int, void *)
#endif
#ifdef __NR_lstat
#define have_sys_lstat
define_syscall2 (__NR_lstat, lstat, sys_lstat, long, char *, void *)
#endif
#ifdef __NR_newlstat
#define have_sys_newlstat
define_syscall2 (__NR_newlstat, newlstat, sys_newlstat, long, char *, void *)
#endif
#ifdef __NR_poll
#define have_sys_poll
define_syscall3 (__NR_poll, poll, sys_poll, long, void *, unsigned int, long)
#endif
#ifdef __NR_lseek
#define have_sys_lseek
define_syscall3 (__NR_lseek, lseek, sys_lseek, int, unsigned int, int, unsigned int)
#endif
#ifdef __NR_mmap
#define have_sys_mmap
define_syscall6 (__NR_mmap, mmap, sys_mmap, void *, void *, int, int, int, int, long)
#endif
#ifdef __NR_mprotect
#define have_sys_mprotect
define_syscall3 (__NR_mprotect, mprotect, sys_mprotect, int, void *, int, int)
#endif
#ifdef __NR_munmap
#define have_sys_munmap
define_syscall2 (__NR_munmap, munmap, sys_munmap, int, void *, int)
#endif
#ifdef __NR_brk
#define have_sys_brk
define_syscall1 (__NR_brk, brk, sys_brk, unsigned long, unsigned long)
#endif
#ifdef __NR_rt_sigaction
#define have_sys_rt_sigaction
define_syscall4 (__NR_rt_sigaction, rt_sigaction, sys_rt_sigaction, long, int, void *, void *, int)
#endif
#ifdef __NR_sigaction
#define have_sys_sigaction
define_syscall3 (__NR_sigaction, rt_sigaction, sys_rt_sigaction, long, int, void *, void *)
#endif
#ifdef __NR_rt_sigprocmask
#define have_sys_rt_sigprocmask
define_syscall4 (__NR_rt_sigprocmask, rt_sigprocmask, sys_rt_sigprocmask, long, int, void *, void *, int)
#endif
#ifdef __NR_sigprocmask
#define have_sys_sigprocmask
define_syscall3 (__NR_sigprocmask, sigprocmask, sys_sigprocmask, long, int, void *, void *)
#endif
#ifdef __NR_rt_sigreturn
#define have_sys_rt_sigreturn
define_syscall0 (__NR_rt_sigreturn, rt_sigreturn, sys_rt_sigreturn, int)
#endif
#ifdef __NR_ioctl
#define have_sys_ioctl
define_syscall3 (__NR_ioctl, ioctl, sys_ioctl, long, unsigned int, unsigned int, unsigned long)
#endif
#ifdef __NR_pread
#define have_sys_pread
define_syscall4 (__NR_pread, pread, sys_pread, int, unsigned int, void *, int, int)
#endif
#ifdef __NR_pwrite
#define have_sys_pwrite
define_syscall4 (__NR_pwrite, pwrite, sys_pwrite, int, unsigned int, void *, int, int)
#endif
#ifdef __NR_pread
#define have_sys_pread64
define_syscall4 (__NR_pread64, pread64, sys_pread64, int, unsigned int, void *, int, int)
#endif
#ifdef __NR_pwrite
#define have_sys_pwrite64
define_syscall4 (__NR_pwrite64, pwrite64, sys_pwrite64, int, unsigned int, void *, int, int)
#endif
#ifdef __NR_readv
#define have_sys_readv
define_syscall3 (__NR_readv, readv, sys_readv, int, unsigned long, void *, unsigned long)
#endif
#ifdef __NR_writev
#define have_sys_writev
define_syscall3 (__NR_writev, writev, sys_writev, int, unsigned long, void *, unsigned long)
#endif
#ifdef __NR_access
#define have_sys_access
define_syscall2 (__NR_access, access, sys_access, long, const char *, int)
#endif
#ifdef __NR_pipe
#define have_sys_pipe
define_syscall1 (__NR_pipe, pipe, sys_pipe, long, int *)
#endif
#ifdef __NR_select
#define have_sys_select
define_syscall5 (__NR_select, select, sys_select, long, int, void *, void *, void *, void *)
#endif
#ifdef __NR_sched_yield
#define have_sys_sched_yield
define_syscall0 (__NR_sched_yield, sched_yield, sys_sched_yield, long)
#endif
#ifdef __NR_mremap
#define have_sys_mremap
define_syscall5 (__NR_mremap, mremap, sys_mremap, void *, void *, unsigned long, unsigned long, unsigned long, void *)
#endif
#ifdef __NR_msync
#define have_sys_msync
define_syscall3 (__NR_msync, msync, sys_msync, long, unsigned long, int, int)
#endif
#ifdef __NR_mincore
#define have_sys_mincore
define_syscall3 (__NR_mincore, mincore, sys_mincore, long, unsigned long, int, unsigned char *)
#endif
#ifdef __NR_madvise
#define have_sys_madvise
define_syscall3 (__NR_madvise, madvise, sys_madvise, long, unsigned long, int, int)
#endif
#ifdef __NR_shmget
#define have_sys_shmget
define_syscall3 (__NR_shmget, shmget, sys_shmget, long, int, int, int)
#endif
#ifdef __NR_shmat
#define have_sys_shmat
define_syscall3 (__NR_shmat, shmat, sys_shmat, long, int, char *, int)
#endif
#ifdef __NR_shmctl
#define have_sys_shmctl
define_syscall3 (__NR_shmctl, shmctl, sys_shmctl, long, int, int, void *)
#endif
#ifdef __NR_dup
#define have_sys_dup
define_syscall1 (__NR_dup, dup, sys_dup, long, unsigned int)
#endif
#ifdef __NR_dup
#define have_sys_dup2
define_syscall2 (__NR_dup2, dup2, sys_dup2, long, unsigned int, unsigned int)
#endif
#ifdef __NR_pause
#define have_sys_pause
define_syscall0 (__NR_pause, pause, sys_pause, long)
#endif
#ifdef __NR_nanosleep
#define have_sys_nanosleep
define_syscall2 (__NR_nanosleep, nanosleep, sys_nanosleep, long, void *, void *)
#endif
#ifdef __NR_getitimer
#define have_sys_getitimer
define_syscall2 (__NR_getitimer, getitimer, sys_getitimer, long, int, void *)
#endif
#ifdef __NR_alarm
#define have_sys_alarm
define_syscall1 (__NR_alarm, alarm, sys_alarm, unsigned long, unsigned int)
#endif
#ifdef __NR_setitimer
#define have_sys_setitimer
define_syscall3 (__NR_setitimer, setitimer, sys_setitimer, long, int, void *, void *)
#endif
#ifdef __NR_getpid
#define have_sys_getpid
define_syscall0 (__NR_getpid, getpid, sys_getpid, long)
#endif
#ifdef __NR_sendfile
#define have_sys_sendfile
define_syscall4 (__NR_sendfile, sendfile, sys_sendfile, int, int, int, int *, int)
#endif
#ifdef __NR_sendfile64
#define have_sys_sendfile64
define_syscall4 (__NR_sendfile64, sendfile64, sys_sendfile64, int, int, int, int *, int)
#endif
#ifdef __NR_socket
#define have_sys_socket
define_syscall3 (__NR_socket, socket, sys_socket, long, int, int, int)
#endif
#ifdef __NR_connect
#define have_sys_connect
define_syscall3 (__NR_connect, connect, sys_connect, long, int, void *, int)
#endif
#ifdef __NR_accept
#define have_sys_accept
define_syscall3 (__NR_accept, accept, sys_accept, long, int, void *, int *)
#endif
#ifdef __NR_sendto
#define have_sys_sendto
define_syscall6 (__NR_sendto, sendto, sys_sendto, long, int, void *, int, unsigned int, void *, int)
#endif
#ifdef __NR_recvfrom
#define have_sys_recvfrom
define_syscall6 (__NR_recvfrom, recvfrom, sys_recvfrom, long, int, void *, int, unsigned int, void *, int *)
#endif
#ifdef __NR_sendmsg
#define have_sys_sendmsg
define_syscall3 (__NR_sendmsg, sendmsg, sys_sendmsg, long, int, void *, unsigned int)
#endif
#ifdef __NR_recvmsg
#define have_sys_recvmsg
define_syscall3 (__NR_recvmsg, recvmsg, sys_recvmsg, long, int, void *, unsigned int)
#endif
#ifdef __NR_shutdown
#define have_sys_shutdown
define_syscall2 (__NR_shutdown, shutdown, sys_shutdown, long, int, int)
#endif
#ifdef __NR_bind
#define have_sys_bind
define_syscall3 (__NR_bind, bind, sys_bind, long, int, void *, int)
#endif
#ifdef __NR_listen
#define have_sys_listen
define_syscall2 (__NR_listen, listen, sys_listen, long, int, int)
#endif
#ifdef __NR_getsockname
#define have_sys_getsockname
define_syscall3 (__NR_getsockname, getsockname, sys_getsockname, long, int, void *, int *)
#endif
#ifdef __NR_getpeername
#define have_sys_getpeername
define_syscall3 (__NR_getpeername, getpeername, sys_getpeername, long, int, void *, int *)
#endif
#ifdef __NR_socketpair
#define have_sys_socketpair
define_syscall4 (__NR_socketpair, socketpair, sys_socketpair, long, int, int, int, int *)
#endif
#ifdef __NR_setsockopt
#define have_sys_setsockopt
define_syscall5 (__NR_setsockopt, setsockopt, sys_setsockopt, long, int, int, int, char *, int)
#endif
#ifdef __NR_getsockopt
#define have_sys_getsockopt
define_syscall5 (__NR_getsockopt, getsockopt, sys_getsockopt, long, int, int, int, char *, int *)
#endif
#ifdef __NR_clone
#define have_sys_clone
define_syscall5 (__NR_clone, clone, sys_clone, int, unsigned, void *, void *, void *, void *)
#endif
#ifdef __NR_fork
#define have_sys_fork
define_syscall1 (__NR_fork, fork, sys_fork, int, void *)
#endif
#ifdef __NR_vfork
#define have_sys_vfork
define_syscall0 (__NR_vfork, vfork, sys_vfork, int)
#endif
#ifdef __NR_execve
#define have_sys_execve
define_syscall3 (__NR_execve, execve, sys_execve, long, const char *, char **, char **)
#endif
#ifdef __NR_exit
#define have_sys_exit
define_syscall1 (__NR_exit, exit, sys_exit, long, int)
#endif
#ifdef __NR_wait4
#define have_sys_wait4
define_syscall4 (__NR_wait4, wait4, sys_wait4, long, int, int *, int, void *)
#endif
#ifdef __NR_kill
#define have_sys_kill
define_syscall2 (__NR_kill, kill, sys_kill, long, int, int)
#endif
#ifdef __NR_uname
#define have_sys_uname
define_syscall1 (__NR_uname, uname, sys_uname, long, void *)
#endif
#ifdef __NR_semget
#define have_sys_semget
define_syscall3 (__NR_semget, semget, sys_semget, long, int, int, int)
#endif
#ifdef __NR_semop
#define have_sys_semop
define_syscall3 (__NR_semop, semop, sys_semop, long, int, void *, unsigned)
#endif
#ifdef __NR_semctl
#define have_sys_semctl
define_syscall4 (__NR_semctl, semctl, sys_semctl, long, int, int, int, int)
#endif
#ifdef __NR_shmdt
#define have_sys_shmdt
define_syscall1 (__NR_shmdt, shmdt, sys_shmdt, long, char *)
#endif
#ifdef __NR_msgget
#define have_sys_msgget
define_syscall2 (__NR_msgget, msgget, sys_msgget, long, int, int)
#endif
#ifdef __NR_msgsnd
#define have_sys_msgsnd
define_syscall4 (__NR_msgsnd, msgsnd, sys_msgsnd, long, int, void *, int, int)
#endif
#ifdef __NR_msgrcv
#define have_sys_msgrcv
define_syscall5 (__NR_msgrcv, msgrcv, sys_msgrcv, long, int, void *, int, long, int)
#endif
#ifdef __NR_msgctl
#define have_sys_msgctl
define_syscall3 (__NR_msgctl, msgctl, sys_msgctl, long, int, int, void *)
#endif
#ifdef __NR_fcntl
#define have_sys_fcntl
define_syscall3 (__NR_fcntl, fcntl, sys_fcntl, long, unsigned int, unsigned int, unsigned long)
#endif
#ifdef __NR_flock
#define have_sys_flock
define_syscall2 (__NR_flock, flock, sys_flock, long, unsigned int, unsigned int)
#endif
#ifdef __NR_fsync
#define have_sys_fsync
define_syscall1 (__NR_fsync, fsync, sys_fsync, long, unsigned int)
#endif
#ifdef __NR_fdatasync
#define have_sys_fdatasync
define_syscall1 (__NR_fdatasync, fdatasync, sys_fdatasync, long, unsigned int)
#endif
#ifdef __NR_truncate
#define have_sys_truncate
define_syscall2 (__NR_truncate, truncate, sys_truncate, long, char *, unsigned long)
#endif
#ifdef __NR_ftruncate
#define have_sys_ftruncate
define_syscall2 (__NR_ftruncate, ftruncate, sys_ftruncate, long, unsigned int, unsigned long)
#endif
#ifdef __NR_getdents
#define have_sys_getdents
define_syscall3 (__NR_getdents, getdents, sys_getdents, long, unsigned int, void *, unsigned int)
#endif
#ifdef __NR_getdents64
#define have_sys_getdents64
define_syscall3 (__NR_getdents64, getdents64, sys_getdents64, long, unsigned int, void *, unsigned int)
#endif
#ifdef __NR_getcwd
#define have_sys_getcwd
define_syscall2 (__NR_getcwd, getcwd, sys_getcwd, long, char *, unsigned long)
#endif
#ifdef __NR_chdir
#define have_sys_chdir
define_syscall1 (__NR_chdir, chdir, sys_chdir, long, const char *)
#endif
#ifdef __NR_fchdir
#define have_sys_fchdir
define_syscall1 (__NR_fchdir, fchdir, sys_fchdir, long, unsigned int)
#endif
#ifdef __NR_rename
#define have_sys_rename
define_syscall2 (__NR_rename, rename, sys_rename, long, const char *, const char *)
#endif
#ifdef __NR_mkdir
#define have_sys_mkdir
define_syscall2 (__NR_mkdir, mkdir, sys_mkdir, long, const char *, int)
#endif
#ifdef __NR_rmdir
#define have_sys_rmdir
define_syscall1 (__NR_rmdir, rmdir, sys_rmdir, long, const char *)
#endif
#ifdef __NR_creat
#define have_sys_creat
define_syscall2 (__NR_creat, creat, sys_creat, long, const char *, int)
#endif
#ifdef __NR_link
#define have_sys_link
define_syscall2 (__NR_link, link, sys_link, long, const char *, const char *)
#endif
#ifdef __NR_unlink
#define have_sys_unlink
define_syscall1 (__NR_unlink, unlink, sys_unlink, long, const char *)
#endif
#ifdef __NR_symlink
#define have_sys_symlink
define_syscall2 (__NR_symlink, symlink, sys_symlink, long, const char *, const char *)
#endif
#ifdef __NR_readlink
#define have_sys_readlink
define_syscall3 (__NR_readlink, readlink, sys_readlink, long, const char *, char *, int)
#endif
#ifdef __NR_chmod
#define have_sys_chmod
define_syscall2 (__NR_chmod, chmod, sys_chmod, long, const char *, int)
#endif
#ifdef __NR_fchmod
#define have_sys_fchmod
define_syscall2 (__NR_fchmod, fchmod, sys_fchmod, long, unsigned int, int)
#endif
#ifdef __NR_chown
#define have_sys_chown
define_syscall3 (__NR_chown, chown, sys_chown, long, const char *, int, int)
#endif
#ifdef __NR_fchown
#define have_sys_fchown
define_syscall3 (__NR_fchown, fchown, sys_fchown, long, unsigned int, int, int)
#endif
#ifdef __NR_lchown
#define have_sys_lchown
define_syscall3 (__NR_lchown, lchown, sys_lchown, long, const char *, int, int)
#endif
#ifdef __NR_umask
#define have_sys_umask
define_syscall1 (__NR_umask, umask, sys_umask, long, int)
#endif
#ifdef __NR_gettimeofday
#define have_sys_gettimeofday
define_syscall2 (__NR_gettimeofday, gettimeofday, sys_gettimeofday, long, void *, void *)
#endif
#ifdef __NR_getrlimit
#define have_sys_getrlimit
define_syscall2 (__NR_getrlimit, getrlimit, sys_getrlimit, long, unsigned int, void *)
#endif
#ifdef __NR_getrusage
#define have_sys_getrusage
define_syscall2 (__NR_getrusage, getrusage, sys_getrusage, long, int, void *)
#endif
#ifdef __NR_sysinfo
#define have_sys_sysinfo
define_syscall1 (__NR_sysinfo, sysinfo, sys_sysinfo, long, void *)
#endif
#ifdef __NR_times
#define have_sys_times
define_syscall1 (__NR_times, times, sys_times, long, void *)
#endif
#ifdef __NR_ptrace
#define have_sys_ptrace
define_syscall4 (__NR_ptrace, ptrace, sys_ptrace, long, long, long, long, long)
#endif
#ifdef __NR_getuid
#define have_sys_getuid
define_syscall0 (__NR_getuid, getuid, sys_getuid, long)
#endif
#ifdef __NR_syslog
#define have_sys_syslog
define_syscall3 (__NR_syslog, syslog, sys_syslog, long, int, char *, int)
#endif
#ifdef __NR_getgid
#define have_sys_getgid
define_syscall0 (__NR_getgid, getgid, sys_getgid, long)
#endif
#ifdef __NR_setuid
#define have_sys_setuid
define_syscall1 (__NR_setuid, setuid, sys_setuid, long, int)
#endif
#ifdef __NR_setgid
#define have_sys_setgid
define_syscall1 (__NR_setgid, setgid, sys_setgid, long, int)
#endif
#ifdef __NR_geteuid
#define have_sys_geteuid
define_syscall0 (__NR_geteuid, geteuid, sys_geteuid, long)
#endif
#ifdef __NR_getegid
#define have_sys_getegid
define_syscall0 (__NR_getegid, getegid, sys_getegid, long)
#endif
#ifdef __NR_setpgid
#define have_sys_setpgid
define_syscall2 (__NR_setpgid, setpgid, sys_setpgid, long, int, int)
#endif
#ifdef __NR_getppid
#define have_sys_getppid
define_syscall0 (__NR_getppid, getppid, sys_getppid, long)
#endif
#ifdef __NR_getpgrp
#define have_sys_getpgrp
define_syscall0 (__NR_getpgrp, getpgrp, sys_getpgrp, long)
#endif
#ifdef __NR_setsid
#define have_sys_setsid
define_syscall0 (__NR_setsid, setsid, sys_setsid, long)
#endif
#ifdef __NR_setreuid
#define have_sys_setreuid
define_syscall2 (__NR_setreuid, setreuid, sys_setreuid, long, int, int)
#endif
#ifdef __NR_setregid
#define have_sys_setregid
define_syscall2 (__NR_setregid, setregid, sys_setregid, long, int, int)
#endif
#ifdef __NR_getgroups
#define have_sys_getgroups
define_syscall2 (__NR_getgroups, getgroups, sys_getgroups, long, int, int *)
#endif
#ifdef __NR_setgroups
#define have_sys_setgroups
define_syscall2 (__NR_setgroups, setgroups, sys_setgroups, long, int, int *)
#endif
#ifdef __NR_setresuid
#define have_sys_setresuid
define_syscall3 (__NR_setresuid, setresuid, sys_setresuid, long, int, int, int)
#endif
#ifdef __NR_getresuid
#define have_sys_getresuid
define_syscall3 (__NR_getresuid, getresuid, sys_getresuid, long, int *, int *, int *)
#endif
#ifdef __NR_setresgid
#define have_sys_setresgid
define_syscall3 (__NR_setresgid, setresgid, sys_setresgid, long, int, int, int)
#endif
#ifdef __NR_getresgid
#define have_sys_getresgid
define_syscall3 (__NR_getresgid, getresgid, sys_getresgid, long, int *, int *, int *)
#endif
#ifdef __NR_getpgid
#define have_sys_getpgid
define_syscall1 (__NR_getpgid, getpgid, sys_getpgid, long, int)
#endif
#ifdef __NR_setfsuid
#define have_sys_setfsuid
define_syscall1 (__NR_setfsuid, setfsuid, sys_setfsuid, long, int)
#endif
#ifdef __NR_setfsgid
#define have_sys_setfsgid
define_syscall1 (__NR_setfsgid, setfsgid, sys_setfsgid, long, int)
#endif
#ifdef __NR_getsid
#define have_sys_getsid
define_syscall1 (__NR_getsid, getsid, sys_getsid, long, int)
#endif
#ifdef __NR_capget
#define have_sys_capget
define_syscall2 (__NR_capget, capget, sys_capget, long, void *, void *)
#endif
#ifdef __NR_capset
#define have_sys_capset
define_syscall2 (__NR_capset, capset, sys_capset, long, void *, void *)
#endif
#ifdef __NR_rt_sigpending
#define have_sys_rt_sigpending
define_syscall2 (__NR_rt_sigpending, rt_sigpending, sys_rt_sigpending, long, void *, int)
#endif
#ifdef __NR_rt_sigtimedwait
#define have_sys_rt_sigtimedwait
define_syscall4 (__NR_rt_sigtimedwait, rt_sigtimedwait, sys_rt_sigtimedwait, long, const void *, void *, const void *, int)
#endif
#ifdef __NR_rt_sigqueueinfo
#define have_sys_rt_sigqueueinfo
define_syscall3 (__NR_rt_sigqueueinfo, rt_sigqueueinfo, sys_rt_sigqueueinfo, long, int, int, void *)
#endif
#ifdef __NR_rt_sigsuspend
#define have_sys_rt_sigsuspend
define_syscall2 (__NR_rt_sigsuspend, rt_sigsuspend, sys_rt_sigsuspend, long, void *, int)
#endif
#ifdef __NR_sigaltstack
#define have_sys_sigaltstack
define_syscall3 (__NR_sigaltstack, sigaltstack, sys_sigaltstack, long, const void *, void *, unsigned long)
#endif
#ifdef __NR_utime
#define have_sys_utime
define_syscall2 (__NR_utime, utime, sys_utime, long, char *, void *)
#endif
#ifdef __NR_mknod
#define have_sys_mknod
define_syscall3 (__NR_mknod, mknod, sys_mknod, long, const char *, int, unsigned int)
#endif
#ifdef __NR_uselib
#define have_sys_uselib
define_syscall1 (__NR_uselib, uselib, sys_uselib, long, const char *)
#endif
#ifdef __NR_personality
#define have_sys_personality
define_syscall1 (__NR_personality, personality, sys_personality, long, unsigned long)
#endif
#ifdef __NR_ustat
#define have_sys_ustat
define_syscall2 (__NR_ustat, ustat, sys_ustat, long, unsigned int, void *)
#endif
#ifdef __NR_statfs
#define have_sys_statfs
define_syscall2 (__NR_statfs, statfs, sys_statfs, long, const char *, void *)
#endif
#ifdef __NR_statfs64
#define have_sys_statfs64
define_syscall2 (__NR_statfs64, statfs64, sys_statfs64, long, const char *, void *)
#endif
#ifdef __NR_fstatfs
#define have_sys_fstatfs
define_syscall2 (__NR_fstatfs, fstatfs, sys_fstatfs, long, unsigned int, void *)
#endif
#ifdef __NR_fstatfs64
#define have_sys_fstatfs64
define_syscall2 (__NR_fstatfs64, fstatfs64, sys_fstatfs64, long, unsigned int, void *)
#endif
#ifdef __NR_sysfs
#define have_sys_sysfs
define_syscall3 (__NR_sysfs, sysfs, sys_sysfs, long, int, unsigned long, unsigned long)
#endif
#ifdef __NR_getpriority
#define have_sys_getpriority
define_syscall2 (__NR_getpriority, getpriority, sys_getpriority, long, int, int)
#endif
#ifdef __NR_setpriority
#define have_sys_setpriority
define_syscall3 (__NR_setpriority, setpriority, sys_setpriority, long, int, int, int)
#endif
#ifdef __NR_sched_setparam
#define have_sys_sched_setparam
define_syscall2 (__NR_sched_setparam, sched_setparam, sys_sched_setparam, long, int, void *)
#endif
#ifdef __NR_sched_getparam
#define have_sys_sched_getparam
define_syscall2 (__NR_sched_getparam, sched_getparam, sys_sched_getparam, long, int, void *)
#endif
#ifdef __NR_sched_setscheduler
#define have_sys_sched_setscheduler
define_syscall3 (__NR_sched_setscheduler, sched_setscheduler, sys_sched_setscheduler, long, int, int, void *)
#endif
#ifdef __NR_sched_getscheduler
#define have_sys_sched_getscheduler
define_syscall1 (__NR_sched_getscheduler, sched_getscheduler, sys_sched_getscheduler, long, int)
#endif
#ifdef __NR_sched_get_priority_max
#define have_sys_sched_get_priority_max
define_syscall1 (__NR_sched_get_priority_max, sched_get_priority_max, sys_sched_get_priority_max, long, int)
#endif
#ifdef __NR_sched_get_priority_min
#define have_sys_sched_get_priority_min
define_syscall1 (__NR_sched_get_priority_min, sched_get_priority_min, sys_sched_get_priority_min, long, int)
#endif
#ifdef __NR_sched_rr_get_interval
#define have_sys_sched_rr_get_interval
define_syscall2 (__NR_sched_rr_get_interval, sched_rr_get_interval, sys_sched_rr_get_interval, long, int, void *)
#endif
#ifdef __NR_mlock
#define have_sys_mlock
define_syscall2 (__NR_mlock, mlock, sys_mlock, long, unsigned long, int)
#endif
#ifdef __NR_munlock
#define have_sys_munlock
define_syscall2 (__NR_munlock, munlock, sys_munlock, long, unsigned long, int)
#endif
#ifdef __NR_mlockall
#define have_sys_mlockall
define_syscall1 (__NR_mlockall, mlockall, sys_mlockall, long, int)
#endif
#ifdef __NR_munlockall
#define have_sys_munlockall
define_syscall0 (__NR_munlockall, munlockall, sys_munlockall, long)
#endif
#ifdef __NR_vhangup
#define have_sys_vhangup
define_syscall0 (__NR_vhangup, vhangup, sys_vhangup, long)
#endif
#ifdef __NR_modify_ldt
#define have_sys_modify_ldt
define_syscall3 (__NR_modify_ldt, modify_ldt, sys_modify_ldt, long, int, void *, unsigned long)
#endif
#ifdef __NR_pivot_root
#define have_sys_pivot_root
define_syscall2 (__NR_pivot_root, pivot_root, sys_pivot_root, long, const char *, const char *)
#endif
#ifdef __NR__sysctl
#define have_sys__sysctl
define_syscall1 (__NR__sysctl, _sysctl, sys__sysctl, long, void *)
#endif
#ifdef __NR_prctl
#define have_sys_prctl
define_syscall5 (__NR_prctl, prctl, sys_prctl, long, int, unsigned long, unsigned long, unsigned long, unsigned long)
#endif
#ifdef __NR_arch_prctl
#define have_sys_arch_prctl
define_syscall2 (__NR_arch_prctl, arch_prctl, sys_arch_prctl, long, int, unsigned long)
#endif
#ifdef __NR_adjtimex
#define have_sys_adjtimex
define_syscall1 (__NR_adjtimex, adjtimex, sys_adjtimex, long, void *)
#endif
#ifdef __NR_setrlimit
#define have_sys_setrlimit
define_syscall2 (__NR_setrlimit, setrlimit, sys_setrlimit, long, unsigned int, void *)
#endif
#ifdef __NR_chroot
#define have_sys_chroot
define_syscall1 (__NR_chroot, chroot, sys_chroot, long, const char *)
#endif
#ifdef __NR_sync
#define have_sys_sync
define_syscall0 (__NR_sync, sync, sys_sync, long)
#endif
#ifdef __NR_acct
#define have_sys_acct
define_syscall1 (__NR_acct, acct, sys_acct, long, const char *)
#endif
#ifdef __NR_settimeofday
#define have_sys_settimeofday
define_syscall2 (__NR_settimeofday, settimeofday, sys_settimeofday, long, void *, void *)
#endif
#ifdef __NR_mount
#define have_sys_mount
define_syscall5 (__NR_mount, mount, sys_mount, long, char *, char *, char *, unsigned long, void *)
#endif
#ifdef __NR_umount
#define have_sys_umount
define_syscall1 (__NR_umount, umount, sys_umount, long, char *)
#endif
#ifdef __NR_umount2
#define have_sys_umount2
define_syscall2 (__NR_umount2, umount2, sys_umount2, long, char *, int)
#endif
#ifdef __NR_swapon
#define have_sys_swapon
define_syscall2 (__NR_swapon, swapon, sys_swapon, long, const char *, int)
#endif
#ifdef __NR_swapoff
#define have_sys_swapoff
define_syscall1 (__NR_swapoff, swapoff, sys_swapoff, long, const char *)
#endif
#ifdef __NR_reboot
#define have_sys_reboot
define_syscall4 (__NR_reboot, reboot, sys_reboot, long, int, int, unsigned int, void *)
#endif
#ifdef __NR_sethostname
#define have_sys_sethostname
define_syscall2 (__NR_sethostname, sethostname, sys_sethostname, long, char *, int)
#endif
#ifdef __NR_setdomainname
#define have_sys_setdomainname
define_syscall2 (__NR_setdomainname, setdomainname, sys_setdomainname, long, char *, int)
#endif
#ifdef __NR_iopl
#define have_sys_iopl
define_syscall2 (__NR_iopl, iopl, sys_iopl, long, unsigned long, void *)
#endif
#ifdef __NR_ioperm
#define have_sys_ioperm
define_syscall3 (__NR_ioperm, ioperm, sys_ioperm, long, unsigned long, unsigned long, int)
#endif
#if 0
#ifdef __NR_create_module
#define have_sys_create_module
define_syscall0 (__NR_create_module, create_module, sys_create_module, int)
#endif
#endif
#ifdef __NR_init_module
#define have_sys_init_module
define_syscall3 (__NR_init_module, init_module, sys_init_module, long, void *, unsigned long, const char *)
#endif
#ifdef __NR_delete_module
#define have_sys_delete_module
define_syscall2 (__NR_delete_module, delete_module, sys_delete_module, long, const char *, unsigned int)
#endif
#if 0
#ifdef __NR_get_kernel_syms
#define have_sys_get_kernel_syms
define_syscall0 (__NR_get_kernel_syms, get_kernel_syms, sys_get_kernel_syms, int)
#endif
#ifdef __NR_query_module
#define have_sys_query_module
define_syscall0 (__NR_query_module, query_module, sys_query_module, int)
#endif
#endif
#ifdef __NR_quotactl
#define have_sys_quotactl
define_syscall4 (__NR_quotactl, quotactl, sys_quotactl, long, unsigned int, const char *, int, void *)
#endif
#ifdef __NR_nfsservctl
#define have_sys_nfsservctl
define_syscall3 (__NR_nfsservctl, nfsservctl, sys_nfsservctl, long, int, void *, void *)
#endif
#ifdef __NR_getpmsg
#define have_sys_getpmsg
define_syscall5 (__NR_getpmsg, getpmsg, sys_getpmsg, long, int, void *, void *, int *, int *)
#endif
#ifdef __NR_putpmsg
#define have_sys_putpmsg
define_syscall4 (__NR_putpmsg, putpmsg, sys_putpmsg, long, int, const void *, const void *, int)
#endif
#if 0
#ifdef __NR_afs_syscall
#define have_sys_afs_syscall
define_syscall0 (__NR_afs_syscall, afs_syscall, sys_afs_syscall, int)
#endif
#ifdef __NR_tuxcall
#define have_sys_tuxcall
define_syscall0 (__NR_tuxcall, tuxcall, sys_tuxcall, int)
#endif
#ifdef __NR_security
#define have_sys_security
define_syscall0 (__NR_security, security, sys_security, int)
#endif
#endif
#ifdef __NR_gettid
#define have_sys_gettid
define_syscall0 (__NR_gettid, gettid, sys_gettid, long)
#endif
#ifdef __NR_readahead
#define have_sys_readahead
define_syscall3 (__NR_readahead, readahead, sys_readahead, long, int, int, int)
#endif
#ifdef __NR_setxattr
#define have_sys_setxattr
define_syscall5 (__NR_setxattr, setxattr, sys_setxattr, long, const char *, const char *, const void *, int, int)
#endif
#ifdef __NR_lsetxattr
#define have_sys_lsetxattr
define_syscall5 (__NR_lsetxattr, lsetxattr, sys_lsetxattr, long, const char *, const char *, const void *, int, int)
#endif
#ifdef __NR_fsetxattr
#define have_sys_fsetxattr
define_syscall5 (__NR_fsetxattr, fsetxattr, sys_fsetxattr, long, int, const char *, const void *, int, int)
#endif
#ifdef __NR_getxattr
#define have_sys_getxattr
define_syscall4 (__NR_getxattr, getxattr, sys_getxattr, long, const char *, const char *, void *, int)
#endif
#ifdef __NR_lgetxattr
#define have_sys_lgetxattr
define_syscall4 (__NR_lgetxattr, lgetxattr, sys_lgetxattr, long, const char *, const char *, void *, int)
#endif
#ifdef __NR_fgetxattr
#define have_sys_fgetxattr
define_syscall4 (__NR_fgetxattr, fgetxattr, sys_fgetxattr, long, int, const char *, void *, int)
#endif
#ifdef __NR_listxattr
#define have_sys_listxattr
define_syscall3 (__NR_listxattr, listxattr, sys_listxattr, long, const char *, char *, int)
#endif
#ifdef __NR_llistxattr
#define have_sys_llistxattr
define_syscall3 (__NR_llistxattr, llistxattr, sys_llistxattr, long, const char *, char *, int)
#endif
#ifdef __NR_flistxattr
#define have_sys_flistxattr
define_syscall3 (__NR_flistxattr, flistxattr, sys_flistxattr, long, int, char *, int)
#endif
#ifdef __NR_removexattr
#define have_sys_removexattr
define_syscall2 (__NR_removexattr, removexattr, sys_removexattr, long, const char *, const char *)
#endif
#ifdef __NR_lremovexattr
#define have_sys_lremovexattr
define_syscall2 (__NR_lremovexattr, lremovexattr, sys_lremovexattr, long, const char *, const char *)
#endif
#ifdef __NR_fremovexattr
#define have_sys_fremovexattr
define_syscall2 (__NR_fremovexattr, fremovexattr, sys_fremovexattr, long, int, const char *)
#endif
#ifdef __NR_tkill
#define have_sys_tkill
define_syscall2 (__NR_tkill, tkill, sys_tkill, long, int, int)
#endif
#ifdef __NR_time
#define have_sys_time
define_syscall1 (__NR_time, time, sys_time, long, void *)
#endif
#ifdef __NR_futex
#define have_sys_futex
define_syscall6 (__NR_futex, futex, sys_futex, long, int *, int, int, void *, int *, int)
#endif
#ifdef __NR_sched_setaffinity
#define have_sys_sched_setaffinity
define_syscall3 (__NR_sched_setaffinity, sched_setaffinity, sys_sched_setaffinity, long, int, unsigned int, unsigned long *)
#endif
#ifdef __NR_sched_getaffinity
#define have_sys_sched_getaffinity
define_syscall3 (__NR_sched_getaffinity, sched_getaffinity, sys_sched_getaffinity, long, int, unsigned int, unsigned long *)
#endif
#ifdef __NR_set_thread_area
#define have_sys_set_thread_area
define_syscall1 (__NR_set_thread_area, set_thread_area, sys_set_thread_area, long, void *)
#endif
#ifdef __NR_io_setup
#define have_sys_io_setup
define_syscall2 (__NR_io_setup, io_setup, sys_io_setup, long, unsigned int, void *)
#endif
#ifdef __NR_io_destroy
#define have_sys_io_destroy
define_syscall1 (__NR_io_destroy, io_destroy, sys_io_destroy, long, long)
#endif
#ifdef __NR_io_getevents
#define have_sys_io_getevents
define_syscall5 (__NR_io_getevents, io_getevents, sys_io_getevents, long, long, long, long, void *, void *)
#endif
#ifdef __NR_io_submit
#define have_sys_io_submit
define_syscall3 (__NR_io_submit, io_submit, sys_io_submit, long, long, long, void **)
#endif
#ifdef __NR_io_cancel
#define have_sys_io_cancel
define_syscall3 (__NR_io_cancel, io_cancel, sys_io_cancel, long, long, void *, void *)
#endif
#ifdef __NR_get_thread_area
#define have_sys_get_thread_area
define_syscall1 (__NR_get_thread_area, get_thread_area, sys_get_thread_area, int, void *)
#endif
#ifdef __NR_lookup_dcookie
#define have_sys_lookup_dcookie
define_syscall3 (__NR_lookup_dcookie, lookup_dcookie, sys_lookup_dcookie, long, long long, char *, int)
#endif
#ifdef __NR_epoll_create
#define have_sys_epoll_create
define_syscall1 (__NR_epoll_create, epoll_create, sys_epoll_create, long, int)
#endif
#ifdef __NR_epoll_ctl_old
#define have_sys_epoll_ctl_old
define_syscall4 (__NR_epoll_ctl_old, epoll_ctl_old, sys_epoll_ctl_old, long, int, int, int, void *)
#endif
#ifdef __NR_epoll_wait_old
#define have_sys_epoll_wait_old
define_syscall4 (__NR_epoll_wait_old, epoll_wait_old, sys_epoll_wait_old, long, int, void *, int, int)
#endif
#ifdef __NR_epoll_ctl
#define have_sys_epoll_ctl
define_syscall4 (__NR_epoll_ctl, epoll_ctl, sys_epoll_ctl, long, int, int, int, void *)
#endif
#ifdef __NR_epoll_wait
#define have_sys_epoll_wait
define_syscall4 (__NR_epoll_wait, epoll_wait, sys_epoll_wait, long, int, void *, int, int)
#endif
#ifdef __NR_remap_file_pages
#define have_sys_remap_file_pages
define_syscall5 (__NR_remap_file_pages, remap_file_pages, sys_remap_file_pages, long, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long)
#endif
#ifdef __NR_set_tid_address
#define have_sys_set_tid_address
define_syscall1 (__NR_set_tid_address, set_tid_address, sys_set_tid_address, long, int *)
#endif
#ifdef __NR_restart_syscall
#define have_sys_restart_syscall
define_syscall0 (__NR_restart_syscall, restart_syscall, sys_restart_syscall, long)
#endif
#ifdef __NR_semtimedop
#define have_sys_semtimedop
define_syscall4 (__NR_semtimedop, semtimedop, sys_semtimedop, long, int, void *, unsigned int, const void *)
#endif
#ifdef __NR_fadvise
#define have_sys_fadvise
define_syscall4 (__NR_fadvise, fadvise, sys_fadvise, long, int, int, int, int)
#endif
#ifdef __NR_fadvise64
#define have_sys_fadvise64
define_syscall4 (__NR_fadvise64, fadvise64, sys_fadvise64, long, int, long, long, int)
#endif
#ifdef __NR_fadvise64_64
#define have_sys_fadvise64_64
define_syscall4 (__NR_fadvise64_64, fadvise64_64, sys_fadvise64_64, long, int, long, long, int)
#endif
#ifdef __NR_timer_create
#define have_sys_timer_create
define_syscall3 (__NR_timer_create, timer_create, sys_timer_create, long, unsigned int, void *, void *)
#endif
#ifdef __NR_timer_settime
#define have_sys_timer_settime
define_syscall4 (__NR_timer_settime, timer_settime, sys_timer_settime, long, unsigned int, int, const void *, void *)
#endif
#ifdef __NR_timer_gettime
#define have_sys_timer_gettime
define_syscall2 (__NR_timer_gettime, timer_gettime, sys_timer_gettime, long, unsigned int, void *)
#endif
#ifdef __NR_timer_getoverrun
#define have_sys_timer_getoverrun
define_syscall1 (__NR_timer_getoverrun, timer_getoverrun, sys_timer_getoverrun, long, unsigned int)
#endif
#ifdef __NR_timer_delete
#define have_sys_timer_delete
define_syscall1 (__NR_timer_delete, timer_delete, sys_timer_delete, long, unsigned int)
#endif
#ifdef __NR_clock_settime
#define have_sys_clock_settime
define_syscall2 (__NR_clock_settime, clock_settime, sys_clock_settime, long, unsigned int, const void *)
#endif
#ifdef __NR_clock_gettime
#define have_sys_clock_gettime
define_syscall2 (__NR_clock_gettime, clock_gettime, sys_clock_gettime, long, unsigned int, void *)
#endif
#ifdef __NR_clock_getres
#define have_sys_clock_getres
define_syscall2 (__NR_clock_getres, clock_getres, sys_clock_getres, long, unsigned int, void *)
#endif
#ifdef __NR_clock_nanosleep
#define have_sys_clock_nanosleep
define_syscall4 (__NR_clock_nanosleep, clock_nanosleep, sys_clock_nanosleep, long, unsigned int, int, const void *, void *)
#endif
#ifdef __NR_exit_group
#define have_sys_exit_group
define_syscall1 (__NR_exit_group, exit_group, sys_exit_group, long, int)
#endif
#ifdef __NR_tgkill
#define have_sys_tgkill
define_syscall3 (__NR_tgkill, tgkill, sys_tgkill, long, int, int, int)
#endif
#ifdef __NR_utimes
#define have_sys_utimes
define_syscall2 (__NR_utimes, utimes, sys_utimes, long, char *, void *)
#endif
#if 0
#ifdef __NR_vserver
#define have_sys_vserver
define_syscall0 (__NR_vserver, vserver, sys_vserver, int)
#endif
#endif
#ifdef __NR_mbind
#define have_sys_mbind
define_syscall6 (__NR_mbind, mbind, sys_mbind, long, unsigned long, unsigned long, unsigned long, unsigned long *, unsigned long, unsigned int)
#endif
#ifdef __NR_set_mempolicy
#define have_sys_set_mempolicy
define_syscall3 (__NR_set_mempolicy, set_mempolicy, sys_set_mempolicy, long, int, unsigned long *, unsigned long)
#endif
#ifdef __NR_get_mempolicy
#define have_sys_get_mempolicy
define_syscall5 (__NR_get_mempolicy, get_mempolicy, sys_get_mempolicy, long, int *, unsigned long *, unsigned long, unsigned long, unsigned long)
#endif
#ifdef __NR_mq_open
#define have_sys_mq_open
define_syscall4 (__NR_mq_open, mq_open, sys_mq_open, long, const char *, int, unsigned int, void *)
#endif
#ifdef __NR_mq_unlink
#define have_sys_mq_unlink
define_syscall1 (__NR_mq_unlink, mq_unlink, sys_mq_unlink, long, const char *)
#endif
#ifdef __NR_mq_timedsend
#define have_sys_mq_timedsend
define_syscall5 (__NR_mq_timedsend, mq_timedsend, sys_mq_timedsend, long, int, const char *, int, unsigned int, const void *)
#endif
#ifdef __NR_mq_timedreceive
#define have_sys_mq_timedreceive
define_syscall5 (__NR_mq_timedreceive, mq_timedreceive, sys_mq_timedreceive, long, int, char *, int, unsigned int *, const void *)
#endif
#ifdef __NR_mq_notify
#define have_sys_mq_notify
define_syscall2 (__NR_mq_notify, mq_notify, sys_mq_notify, long, int, const void *)
#endif
#ifdef __NR_mq_getsetattr
#define have_sys_mq_getsetattr
define_syscall3 (__NR_mq_getsetattr, mq_getsetattr, sys_mq_getsetattr, long, int, const void *, void *)
#endif
#ifdef __NR_kexec_load
#define have_sys_kexec_load
define_syscall4 (__NR_kexec_load, kexec_load, sys_kexec_load, long, unsigned long, unsigned long, void *, unsigned long)
#endif
#ifdef __NR_waitid
#define have_sys_waitid
define_syscall5 (__NR_waitid, waitid, sys_waitid, long, int, int, void *, int, void *)
#endif
#ifdef __NR_add_key
#define have_sys_add_key
define_syscall5 (__NR_add_key, add_key, sys_add_key, long, const char *, const char *, const void *, int, unsigned int)
#endif
#ifdef __NR_request_key
#define have_sys_request_key
define_syscall4 (__NR_request_key, request_key, sys_request_key, long, const char *, const char *, const char *, unsigned int)
#endif
#ifdef __NR_keyctl
#define have_sys_keyctl
define_syscall5 (__NR_keyctl, keyctl, sys_keyctl, long, int, unsigned long, unsigned long, unsigned long, unsigned long)
#endif
#ifdef __NR_ioprio_set
#define have_sys_ioprio_set
define_syscall3 (__NR_ioprio_set, ioprio_set, sys_ioprio_set, long, int, int, int)
#endif
#ifdef __NR_ioprio_get
#define have_sys_ioprio_get
define_syscall2 (__NR_ioprio_get, ioprio_get, sys_ioprio_get, long, int, int)
#endif
#ifdef __NR_inotify_init
#define have_sys_inotify_init
define_syscall0 (__NR_inotify_init, inotify_init, sys_inotify_init, long)
#endif
#ifdef __NR_inotify_add_watch
#define have_sys_inotify_add_watch
define_syscall3 (__NR_inotify_add_watch, inotify_add_watch, sys_inotify_add_watch, long, int, const char *, int)
#endif
#ifdef __NR_inotify_rm_watch
#define have_sys_inotify_rm_watch
define_syscall2 (__NR_inotify_rm_watch, inotify_rm_watch, sys_inotify_rm_watch, long, int, int)
#endif
#ifdef __NR_migrate_pages
#define have_sys_migrate_pages
define_syscall4 (__NR_migrate_pages, migrate_pages, sys_migrate_pages, long, int, unsigned long, const unsigned long *, const unsigned long *)
#endif
#ifdef __NR_openat
#define have_sys_openat
define_syscall4 (__NR_openat, openat, sys_openat, long, int, const char *, int, int)
#endif
#ifdef __NR_mkdirat
#define have_sys_mkdirat
define_syscall3 (__NR_mkdirat, mkdirat, sys_mkdirat, long, int, const char *, int)
#endif
#ifdef __NR_mknodat
#define have_sys_mknodat
define_syscall4 (__NR_mknodat, mknodat, sys_mknodat, long, int, const char *, int, unsigned int)
#endif
#ifdef __NR_fchownat
#define have_sys_fchownat
define_syscall5 (__NR_fchownat, fchownat, sys_fchownat, long, int, const char *, int, int, int)
#endif
#ifdef __NR_futimesat
#define have_sys_futimesat
define_syscall3 (__NR_futimesat, futimesat, sys_futimesat, long, int, char *, void *)
#endif
#ifdef __NR_newfstatat
#define have_sys_newfstatat
define_syscall4 (__NR_newfstatat, newfstatat, sys_newfstatat, long, int, char *, void *, int)
#endif
#ifdef __NR_fstatat
#define have_sys_fstatat
define_syscall4 (__NR_fstatat, fstatat, sys_fstatat, long, int, char *, void *, int)
#endif
#ifdef __NR_fstatat64
#define have_sys_fstatat64
define_syscall4 (__NR_fstatat64, fstatat64, sys_fstatat64, long, int, char *, void *, int)
#endif
#ifdef __NR_unlinkat
#define have_sys_unlinkat
define_syscall3 (__NR_unlinkat, unlinkat, sys_unlinkat, long, int, const char *, int)
#endif
#ifdef __NR_renameat
#define have_sys_renameat
define_syscall4 (__NR_renameat, renameat, sys_renameat, long, int, const char *, int, const char *)
#endif
#ifdef __NR_linkat
#define have_sys_linkat
define_syscall5 (__NR_linkat, linkat, sys_linkat, long, int, const char *, int, const char *, int)
#endif
#ifdef __NR_symlinkat
#define have_sys_symlinkat
define_syscall3 (__NR_symlinkat, symlinkat, sys_symlinkat, long, const char *, int, const char *)
#endif
#ifdef __NR_readlinkat
#define have_sys_readlinkat
define_syscall4 (__NR_readlinkat, readlinkat, sys_readlinkat, long, int, const char *, char *, int)
#endif
#ifdef __NR_fchmodat
#define have_sys_fchmodat
define_syscall3 (__NR_fchmodat, fchmodat, sys_fchmodat, long, int, const char *, int)
#endif
#ifdef __NR_faccessat
#define have_sys_faccessat
define_syscall3 (__NR_faccessat, faccessat, sys_faccessat, long, int, const char *, int)
#endif
#ifdef __NR_pselect6
#define have_sys_pselect6
define_syscall6 (__NR_pselect6, pselect6, sys_pselect6, long, int, void *, void *, void *, void *, void *)
#endif
#ifdef __NR_ppoll
#define have_sys_ppoll
define_syscall4 (__NR_ppoll, ppoll, sys_ppoll, long, void *, unsigned int, void *, const void *)
#endif
#ifdef __NR_unshare
#define have_sys_unshare
define_syscall1 (__NR_unshare, unshare, sys_unshare, long, unsigned long)
#endif
#ifdef __NR_set_robust_list
#define have_sys_set_robust_list
define_syscall2 (__NR_set_robust_list, set_robust_list, sys_set_robust_list, long, void *, int)
#endif
#ifdef __NR_get_robust_list
#define have_sys_get_robust_list
define_syscall3 (__NR_get_robust_list, get_robust_list, sys_get_robust_list, long, int, void **, void *)
#endif
#ifdef __NR_splice
#define have_sys_splice
define_syscall6 (__NR_splice, splice, sys_splice, long, int, int *, int, int *, int, unsigned int)
#endif
#ifdef __NR_tee
#define have_sys_tee
define_syscall4 (__NR_tee, tee, sys_tee, long, int, int, int, unsigned int)
#endif
#ifdef __NR_sync_file_range
#define have_sys_sync_file_range
define_syscall4 (__NR_sync_file_range, sync_file_range, sys_sync_file_range, long, int, int, int, unsigned int)
#endif
#ifdef __NR_sync_file_range2
#define have_sys_sync_file_range2
define_syscall4 (__NR_sync_file_range2, sync_file_range2, sys_sync_file_range2, long, int, int, int, unsigned int)
#endif
#ifdef __NR_vmsplice
#define have_sys_vmsplice
define_syscall4 (__NR_vmsplice, vmsplice, sys_vmsplice, long, int, void *, unsigned long, unsigned int)
#endif
#ifdef __NR_move_pages
#define have_sys_move_pages
define_syscall6 (__NR_move_pages, move_pages, sys_move_pages, long, int, unsigned long, const void **, const int *, int *, int)
#endif
#ifdef __NR_utimensat
#define have_sys_utimensat
define_syscall4 (__NR_utimensat, utimensat, sys_utimensat, long, int, char *, void *, int)
#endif
#ifdef __NR_epoll_pwait
#define have_sys_epoll_pwait
define_syscall6 (__NR_epoll_pwait, epoll_pwait, sys_epoll_pwait, long, int, void *, int, int, const void *, int)
#endif
#ifdef __NR_signalfd
#define have_sys_signalfd
define_syscall3 (__NR_signalfd, signalfd, sys_signalfd, long, int, void *, int)
#endif
#if 0
#ifdef __NR_timerfd
#define have_sys_timerfd
define_syscall0 (__NR_timerfd, timerfd, sys_timerfd, int)
#endif
#endif
#ifdef __NR_eventfd
#define have_sys_eventfd
define_syscall1 (__NR_eventfd, eventfd, sys_eventfd, long, unsigned int)
#endif
#ifdef __NR_fallocate
#define have_sys_fallocate
define_syscall4 (__NR_fallocate, fallocate, sys_fallocate, long, int, int, int, int)
#endif

#ifdef __NR_socketcall
#define have_sys_socketcall
define_syscall2 (__NR_socketcall, socketcall, sys_socketcall, long, unsigned long, unsigned long *)

#ifndef define_socketcall0
#define define_socketcall0(a,b,c,r)\
static inline r c ()\
{ unsigned long sc_tmp[6]= { (unsigned long)0, (unsigned long)0, (unsigned long)0, (unsigned long)0, (unsigned long)0, (unsigned long)0 }; return sys_socketcall((unsigned long)a, sc_tmp); }
#endif

#ifndef define_socketcall1
#define define_socketcall1(a,b,c,r,a1)\
static inline r c (a1 p1)\
{ unsigned long sc_tmp[6]= { (unsigned long)p1, (unsigned long)0, (unsigned long)0, (unsigned long)0, (unsigned long)0, (unsigned long)0 }; return sys_socketcall((unsigned long)a, sc_tmp); }
#endif

#ifndef define_socketcall2
#define define_socketcall2(a,b,c,r,a1,a2)\
static inline r c (a1 p1, a2 p2)\
{ unsigned long sc_tmp[6]= { (unsigned long)p1, (unsigned long)p2, (unsigned long)0, (unsigned long)0, (unsigned long)0, (unsigned long)0 }; return sys_socketcall((unsigned long)a, sc_tmp); }
#endif

#ifndef define_socketcall3
#define define_socketcall3(a,b,c,r,a1,a2,a3)\
static inline r c (a1 p1, a2 p2, a3 p3)\
{ unsigned long sc_tmp[6]= { (unsigned long)p1, (unsigned long)p2, (unsigned long)p3, (unsigned long)0, (unsigned long)0, (unsigned long)0 }; return sys_socketcall((unsigned long)a, sc_tmp); }
#endif

#ifndef define_socketcall4
#define define_socketcall4(a,b,c,r,a1,a2,a3,a4)\
static inline r c (a1 p1, a2 p2, a3 p3, a4 p4)\
{ unsigned long sc_tmp[6]= { (unsigned long)p1, (unsigned long)p2, (unsigned long)p3, (unsigned long)p4, (unsigned long)0, (unsigned long)0 }; return sys_socketcall((unsigned long)a, sc_tmp); }
#endif

#ifndef define_socketcall5
#define define_socketcall5(a,b,c,r,a1,a2,a3,a4,a5)\
static inline r c (a1 p1, a2 p2, a3 p3, a4 p4, a5 p5)\
{ unsigned long sc_tmp[6]= { (unsigned long)p1, (unsigned long)p2, (unsigned long)p3, (unsigned long)p4, (unsigned long)p5, (unsigned long)0 }; return sys_socketcall((unsigned long)a, sc_tmp); }
#endif

#ifndef define_socketcall6
#define define_socketcall6(a,b,c,r,a1,a2,a3,a4,a5,a6)\
static inline r c (a1 p1, a2 p2, a3 p3, a4 p4, a5 p5, a6 p6)\
{ unsigned long sc_tmp[6]= { (unsigned long)p1, (unsigned long)p2, (unsigned long)p3, (unsigned long)p4, (unsigned long)p5, (unsigned long)p6 }; return sys_socketcall((unsigned long)a, sc_tmp); }
#endif

#ifndef have_sys_socket
#define have_sys_socket
define_socketcall3 (1, socket, sys_socket, long, int, int, int)
#endif
#ifndef have_sys_bind
#define have_sys_bind
define_socketcall3 (2, bind, sys_bind, long, int, void *, int)
#endif
#ifndef have_sys_connect
#define have_sys_connect
define_socketcall3 (3, connect, sys_connect, long, int, void *, int)
#endif
#ifndef have_sys_listen
#define have_sys_listen
define_socketcall2 (4, listen, sys_listen, long, int, int)
#endif
#ifndef have_sys_accept
#define have_sys_accept
define_socketcall3 (5, accept, sys_accept, long, int, void *, int *)
#endif
#ifndef have_sys_getsockname
#define have_sys_getsockname
define_socketcall3 (6, getsockname, sys_getsockname, long, int, void *, int *)
#endif
#ifndef have_sys_getpeername
#define have_sys_getpeername
define_socketcall3 (7, getpeername, sys_getpeername, long, int, void *, int *)
#endif
#ifndef have_sys_socketpair
#define have_sys_socketpair
define_socketcall4 (8, socketpair, sys_socketpair, long, int, int, int, int *)
#endif
#ifndef have_sys_send
#define have_sys_send
define_socketcall4 (9, send, sys_send, long, int, void *, long, int)
#endif
#ifndef have_sys_recv
#define have_sys_recv
define_socketcall4 (10, recv, sys_recv, long, int, void *, long, int)
#endif
#ifndef have_sys_sendto
#define have_sys_sendto
define_socketcall6 (11, sendto, sys_sendto, long, int, void *, int, unsigned int, void *, int)
#endif
#ifndef have_sys_recvfrom
#define have_sys_recvfrom
define_socketcall6 (12, recvfrom, sys_recvfrom, long, int, void *, int, unsigned int, void *, int *)
#endif
#ifndef have_sys_shutdown
#define have_sys_shutdown
define_socketcall2 (13, shutdown, sys_shutdown, long, int, int)
#endif
#ifndef have_sys_setsockopt
#define have_sys_setsockopt
define_socketcall5 (14, setsockopt, sys_setsockopt, long, int, int, int, char *, int)
#endif
#ifndef have_sys_getsockopt
#define have_sys_getsockopt
define_socketcall5 (15, getsockopt, sys_getsockopt, long, int, int, int, char *, int *)
#endif
#ifndef have_sys_sendmsg
#define have_sys_sendmsg
define_socketcall3 (16, sendmsg, sys_sendmsg, long, int, void *, unsigned int)
#endif
#ifndef have_sys_recvmsg
#define have_sys_recvmsg
define_socketcall3 (17, recvmsg, sys_recvmsg, long, int, void *, unsigned int)
#endif

#endif

#endif
