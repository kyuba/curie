/*
 *  linux/syscall.h
 *  libsyscall
 *
 *  Created by Magnus Deininger on 11/12/2008.
 *  Copyright 2008 Magnus Deininger. All rights reserved.
 *
 */

/*
 * Copyright (c) 2008, Magnus Deininger All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer. *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution. *
 * Neither the name of the project nor the names of its contributors may
 * be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*! \file
 *  \brief Syscall Header (Linux)
 */

#ifndef LIBSYSCALL_SYSCALL_H
#define LIBSYSCALL_SYSCALL_H

#define linux_syscalls

#include <asm/unistd.h>

#ifndef define_syscall0
#define define_syscall0(num,id,rid,rettype)
#endif

#ifndef define_syscall1
#define define_syscall1(num,id,rid,rettype,p1type)
#endif

#ifndef define_syscall2
#define define_syscall2(num,id,rid,rettype,p1type,p2type)
#endif

#ifndef define_syscall3
#define define_syscall3(num,id,rid,rettype,p1type,p2type,p3type)
#endif

#ifndef define_syscall4
#define define_syscall4(num,id,rid,rettype,p1type,p2type,p3type,p4type)
#endif

#ifndef define_syscall5
#define define_syscall5(num,id,rid,rettype,p1type,p2type,p3type,p4type,p5type)
#endif

#ifndef define_syscall6
#define define_syscall6(num,id,rid,rettype,p1type,p2type,p3type,p4type,p5type,p6type)
#endif

#ifdef __NR_read
define_syscall0 (__NR_read, read, linux_read, void)
#endif
#ifdef __NR_write
define_syscall0 (__NR_write, write, linux_write, void)
#endif
#ifdef __NR_open
define_syscall0 (__NR_open, open, linux_open, void)
#endif
#ifdef __NR_close
define_syscall0 (__NR_close, close, linux_close, void)
#endif
#ifdef __NR_stat
define_syscall0 (__NR_stat, stat, linux_stat, void)
#endif
#ifdef __NR_fstat
define_syscall0 (__NR_fstat, fstat, linux_fstat, void)
#endif
#ifdef __NR_lstat
define_syscall0 (__NR_lstat, lstat, linux_lstat, void)
#endif
#ifdef __NR_poll
define_syscall0 (__NR_poll, poll, linux_poll, void)
#endif
#ifdef __NR_lseek
define_syscall0 (__NR_lseek, lseek, linux_lseek, void)
#endif
#ifdef __NR_mmap
define_syscall0 (__NR_mmap, mmap, linux_mmap, void)
#endif
#ifdef __NR_mprotect
define_syscall0 (__NR_mprotect, mprotect, linux_mprotect, void)
#endif
#ifdef __NR_munmap
define_syscall0 (__NR_munmap, munmap, linux_munmap, void)
#endif
#ifdef __NR_brk
define_syscall0 (__NR_brk, brk, linux_brk, void)
#endif
#ifdef __NR_rt_sigaction
define_syscall0 (__NR_rt_sigaction, rt_sigaction, linux_rt_sigaction, void)
#endif
#ifdef __NR_rt_sigprocmask
define_syscall0 (__NR_rt_sigprocmask, rt_sigprocmask, linux_rt_sigprocmask, void)
#endif
#ifdef __NR_rt_sigreturn
define_syscall0 (__NR_rt_sigreturn, rt_sigreturn, linux_rt_sigreturn, void)
#endif
#ifdef __NR_ioctl
define_syscall0 (__NR_ioctl, ioctl, linux_ioctl, void)
#endif
#ifdef __NR_pread
define_syscall0 (__NR_pread, pread, linux_pread, void)
#endif
#ifdef __NR_pwrite
define_syscall0 (__NR_pwrite, pwrite, linux_pwrite, void)
#endif
#ifdef __NR_readv
define_syscall0 (__NR_readv, readv, linux_readv, void)
#endif
#ifdef __NR_writev
define_syscall0 (__NR_writev, writev, linux_writev, void)
#endif
#ifdef __NR_access
define_syscall0 (__NR_access, access, linux_access, void)
#endif
#ifdef __NR_pipe
define_syscall0 (__NR_pipe, pipe, linux_pipe, void)
#endif
#ifdef __NR_select
define_syscall0 (__NR_select, select, linux_select, void)
#endif
#ifdef __NR_sched_yield
define_syscall0 (__NR_sched_yield, sched_yield, linux_sched_yield, void)
#endif
#ifdef __NR_mremap
define_syscall0 (__NR_mremap, mremap, linux_mremap, void)
#endif
#ifdef __NR_msync
define_syscall0 (__NR_msync, msync, linux_msync, void)
#endif
#ifdef __NR_mincore
define_syscall0 (__NR_mincore, mincore, linux_mincore, void)
#endif
#ifdef __NR_madvise
define_syscall0 (__NR_madvise, madvise, linux_madvise, void)
#endif
#ifdef __NR_shmget
define_syscall0 (__NR_shmget, shmget, linux_shmget, void)
#endif
#ifdef __NR_shmat
define_syscall0 (__NR_shmat, shmat, linux_shmat, void)
#endif
#ifdef __NR_shmctl
define_syscall0 (__NR_shmctl, shmctl, linux_shmctl, void)
#endif
#ifdef __NR_dup
define_syscall0 (__NR_dup, dup, linux_dup, void)
#endif
#ifdef __NR_dup2
define_syscall0 (__NR_dup2, dup2, linux_dup2, void)
#endif
#ifdef __NR_pause
define_syscall0 (__NR_pause, pause, linux_pause, void)
#endif
#ifdef __NR_nanosleep
define_syscall0 (__NR_nanosleep, nanosleep, linux_nanosleep, void)
#endif
#ifdef __NR_getitimer
define_syscall0 (__NR_getitimer, getitimer, linux_getitimer, void)
#endif
#ifdef __NR_alarm
define_syscall0 (__NR_alarm, alarm, linux_alarm, void)
#endif
#ifdef __NR_setitimer
define_syscall0 (__NR_setitimer, setitimer, linux_setitimer, void)
#endif
#ifdef __NR_getpid
define_syscall0 (__NR_getpid, getpid, linux_getpid, void)
#endif
#ifdef __NR_sendfile
define_syscall0 (__NR_sendfile, sendfile, linux_sendfile, void)
#endif
#ifdef __NR_socket
define_syscall0 (__NR_socket, socket, linux_socket, void)
#endif
#ifdef __NR_connect
define_syscall0 (__NR_connect, connect, linux_connect, void)
#endif
#ifdef __NR_accept
define_syscall0 (__NR_accept, accept, linux_accept, void)
#endif
#ifdef __NR_sendto
define_syscall0 (__NR_sendto, sendto, linux_sendto, void)
#endif
#ifdef __NR_recvfrom
define_syscall0 (__NR_recvfrom, recvfrom, linux_recvfrom, void)
#endif
#ifdef __NR_sendmsg
define_syscall0 (__NR_sendmsg, sendmsg, linux_sendmsg, void)
#endif
#ifdef __NR_recvmsg
define_syscall0 (__NR_recvmsg, recvmsg, linux_recvmsg, void)
#endif
#ifdef __NR_shutdown
define_syscall0 (__NR_shutdown, shutdown, linux_shutdown, void)
#endif
#ifdef __NR_bind
define_syscall0 (__NR_bind, bind, linux_bind, void)
#endif
#ifdef __NR_listen
define_syscall0 (__NR_listen, listen, linux_listen, void)
#endif
#ifdef __NR_getsockname
define_syscall0 (__NR_getsockname, getsockname, linux_getsockname, void)
#endif
#ifdef __NR_getpeername
define_syscall0 (__NR_getpeername, getpeername, linux_getpeername, void)
#endif
#ifdef __NR_socketpair
define_syscall0 (__NR_socketpair, socketpair, linux_socketpair, void)
#endif
#ifdef __NR_setsockopt
define_syscall0 (__NR_setsockopt, setsockopt, linux_setsockopt, void)
#endif
#ifdef __NR_getsockopt
define_syscall0 (__NR_getsockopt, getsockopt, linux_getsockopt, void)
#endif
#ifdef __NR_clone
define_syscall0 (__NR_clone, clone, linux_clone, void)
#endif
#ifdef __NR_fork
define_syscall0 (__NR_fork, fork, linux_fork, void)
#endif
#ifdef __NR_vfork
define_syscall0 (__NR_vfork, vfork, linux_vfork, void)
#endif
#ifdef __NR_execve
define_syscall0 (__NR_execve, execve, linux_execve, void)
#endif
#ifdef __NR_exit
define_syscall0 (__NR_exit, exit, linux_exit, void)
#endif
#ifdef __NR_wait
define_syscall0 (__NR_wait, wait, linux_wait, void)
#endif
#ifdef __NR_kill
define_syscall0 (__NR_kill, kill, linux_kill, void)
#endif
#ifdef __NR_uname
define_syscall0 (__NR_uname, uname, linux_uname, void)
#endif
#ifdef __NR_semget
define_syscall0 (__NR_semget, semget, linux_semget, void)
#endif
#ifdef __NR_semop
define_syscall0 (__NR_semop, semop, linux_semop, void)
#endif
#ifdef __NR_semctl
define_syscall0 (__NR_semctl, semctl, linux_semctl, void)
#endif
#ifdef __NR_shmdt
define_syscall0 (__NR_shmdt, shmdt, linux_shmdt, void)
#endif
#ifdef __NR_msgget
define_syscall0 (__NR_msgget, msgget, linux_msgget, void)
#endif
#ifdef __NR_msgsnd
define_syscall0 (__NR_msgsnd, msgsnd, linux_msgsnd, void)
#endif
#ifdef __NR_msgrcv
define_syscall0 (__NR_msgrcv, msgrcv, linux_msgrcv, void)
#endif
#ifdef __NR_msgctl
define_syscall0 (__NR_msgctl, msgctl, linux_msgctl, void)
#endif
#ifdef __NR_fcntl
define_syscall0 (__NR_fcntl, fcntl, linux_fcntl, void)
#endif
#ifdef __NR_flock
define_syscall0 (__NR_flock, flock, linux_flock, void)
#endif
#ifdef __NR_fsync
define_syscall0 (__NR_fsync, fsync, linux_fsync, void)
#endif
#ifdef __NR_fdatasync
define_syscall0 (__NR_fdatasync, fdatasync, linux_fdatasync, void)
#endif
#ifdef __NR_truncate
define_syscall0 (__NR_truncate, truncate, linux_truncate, void)
#endif
#ifdef __NR_ftruncate
define_syscall0 (__NR_ftruncate, ftruncate, linux_ftruncate, void)
#endif
#ifdef __NR_getdents
define_syscall0 (__NR_getdents, getdents, linux_getdents, void)
#endif
#ifdef __NR_getcwd
define_syscall0 (__NR_getcwd, getcwd, linux_getcwd, void)
#endif
#ifdef __NR_chdir
define_syscall0 (__NR_chdir, chdir, linux_chdir, void)
#endif
#ifdef __NR_fchdir
define_syscall0 (__NR_fchdir, fchdir, linux_fchdir, void)
#endif
#ifdef __NR_rename
define_syscall0 (__NR_rename, rename, linux_rename, void)
#endif
#ifdef __NR_mkdir
define_syscall0 (__NR_mkdir, mkdir, linux_mkdir, void)
#endif
#ifdef __NR_rmdir
define_syscall0 (__NR_rmdir, rmdir, linux_rmdir, void)
#endif
#ifdef __NR_creat
define_syscall0 (__NR_creat, creat, linux_creat, void)
#endif
#ifdef __NR_link
define_syscall0 (__NR_link, link, linux_link, void)
#endif
#ifdef __NR_unlink
define_syscall0 (__NR_unlink, unlink, linux_unlink, void)
#endif
#ifdef __NR_symlink
define_syscall0 (__NR_symlink, symlink, linux_symlink, void)
#endif
#ifdef __NR_readlink
define_syscall0 (__NR_readlink, readlink, linux_readlink, void)
#endif
#ifdef __NR_chmod
define_syscall0 (__NR_chmod, chmod, linux_chmod, void)
#endif
#ifdef __NR_fchmod
define_syscall0 (__NR_fchmod, fchmod, linux_fchmod, void)
#endif
#ifdef __NR_chown
define_syscall0 (__NR_chown, chown, linux_chown, void)
#endif
#ifdef __NR_fchown
define_syscall0 (__NR_fchown, fchown, linux_fchown, void)
#endif
#ifdef __NR_lchown
define_syscall0 (__NR_lchown, lchown, linux_lchown, void)
#endif
#ifdef __NR_umask
define_syscall0 (__NR_umask, umask, linux_umask, void)
#endif
#ifdef __NR_gettimeofday
define_syscall0 (__NR_gettimeofday, gettimeofday, linux_gettimeofday, void)
#endif
#ifdef __NR_getrlimit
define_syscall0 (__NR_getrlimit, getrlimit, linux_getrlimit, void)
#endif
#ifdef __NR_getrusage
define_syscall0 (__NR_getrusage, getrusage, linux_getrusage, void)
#endif
#ifdef __NR_sysinfo
define_syscall0 (__NR_sysinfo, sysinfo, linux_sysinfo, void)
#endif
#ifdef __NR_times
define_syscall0 (__NR_times, times, linux_times, void)
#endif
#ifdef __NR_ptrace
define_syscall0 (__NR_ptrace, ptrace, linux_ptrace, void)
#endif
#ifdef __NR_getuid
define_syscall0 (__NR_getuid, getuid, linux_getuid, void)
#endif
#ifdef __NR_syslog
define_syscall0 (__NR_syslog, syslog, linux_syslog, void)
#endif
#ifdef __NR_getgid
define_syscall0 (__NR_getgid, getgid, linux_getgid, void)
#endif
#ifdef __NR_setuid
define_syscall0 (__NR_setuid, setuid, linux_setuid, void)
#endif
#ifdef __NR_setgid
define_syscall0 (__NR_setgid, setgid, linux_setgid, void)
#endif
#ifdef __NR_geteuid
define_syscall0 (__NR_geteuid, geteuid, linux_geteuid, void)
#endif
#ifdef __NR_getegid
define_syscall0 (__NR_getegid, getegid, linux_getegid, void)
#endif
#ifdef __NR_setpgid
define_syscall0 (__NR_setpgid, setpgid, linux_setpgid, void)
#endif
#ifdef __NR_getppid
define_syscall0 (__NR_getppid, getppid, linux_getppid, void)
#endif
#ifdef __NR_getpgrp
define_syscall0 (__NR_getpgrp, getpgrp, linux_getpgrp, void)
#endif
#ifdef __NR_setsid
define_syscall0 (__NR_setsid, setsid, linux_setsid, void)
#endif
#ifdef __NR_setreuid
define_syscall0 (__NR_setreuid, setreuid, linux_setreuid, void)
#endif
#ifdef __NR_setregid
define_syscall0 (__NR_setregid, setregid, linux_setregid, void)
#endif
#ifdef __NR_getgroups
define_syscall0 (__NR_getgroups, getgroups, linux_getgroups, void)
#endif
#ifdef __NR_setgroups
define_syscall0 (__NR_setgroups, setgroups, linux_setgroups, void)
#endif
#ifdef __NR_setresuid
define_syscall0 (__NR_setresuid, setresuid, linux_setresuid, void)
#endif
#ifdef __NR_getresuid
define_syscall0 (__NR_getresuid, getresuid, linux_getresuid, void)
#endif
#ifdef __NR_setresgid
define_syscall0 (__NR_setresgid, setresgid, linux_setresgid, void)
#endif
#ifdef __NR_getresgid
define_syscall0 (__NR_getresgid, getresgid, linux_getresgid, void)
#endif
#ifdef __NR_getpgid
define_syscall0 (__NR_getpgid, getpgid, linux_getpgid, void)
#endif
#ifdef __NR_setfsuid
define_syscall0 (__NR_setfsuid, setfsuid, linux_setfsuid, void)
#endif
#ifdef __NR_setfsgid
define_syscall0 (__NR_setfsgid, setfsgid, linux_setfsgid, void)
#endif
#ifdef __NR_getsid
define_syscall0 (__NR_getsid, getsid, linux_getsid, void)
#endif
#ifdef __NR_capget
define_syscall0 (__NR_capget, capget, linux_capget, void)
#endif
#ifdef __NR_capset
define_syscall0 (__NR_capset, capset, linux_capset, void)
#endif
#ifdef __NR_rt_sigpending
define_syscall0 (__NR_rt_sigpending, rt_sigpending, linux_rt_sigpending, void)
#endif
#ifdef __NR_rt_sigtimedwait
define_syscall0 (__NR_rt_sigtimedwait, rt_sigtimedwait, linux_rt_sigtimedwait, void)
#endif
#ifdef __NR_rt_sigqueueinfo
define_syscall0 (__NR_rt_sigqueueinfo, rt_sigqueueinfo, linux_rt_sigqueueinfo, void)
#endif
#ifdef __NR_rt_sigsuspend
define_syscall0 (__NR_rt_sigsuspend, rt_sigsuspend, linux_rt_sigsuspend, void)
#endif
#ifdef __NR_sigaltstack
define_syscall0 (__NR_sigaltstack, sigaltstack, linux_sigaltstack, void)
#endif
#ifdef __NR_utime
define_syscall0 (__NR_utime, utime, linux_utime, void)
#endif
#ifdef __NR_mknod
define_syscall0 (__NR_mknod, mknod, linux_mknod, void)
#endif
#ifdef __NR_uselib
define_syscall0 (__NR_uselib, uselib, linux_uselib, void)
#endif
#ifdef __NR_personality
define_syscall0 (__NR_personality, personality, linux_personality, void)
#endif
#ifdef __NR_ustat
define_syscall0 (__NR_ustat, ustat, linux_ustat, void)
#endif
#ifdef __NR_statfs
define_syscall0 (__NR_statfs, statfs, linux_statfs, void)
#endif
#ifdef __NR_fstatfs
define_syscall0 (__NR_fstatfs, fstatfs, linux_fstatfs, void)
#endif
#ifdef __NR_sysfs
define_syscall0 (__NR_sysfs, sysfs, linux_sysfs, void)
#endif
#ifdef __NR_getpriority
define_syscall0 (__NR_getpriority, getpriority, linux_getpriority, void)
#endif
#ifdef __NR_setpriority
define_syscall0 (__NR_setpriority, setpriority, linux_setpriority, void)
#endif
#ifdef __NR_sched_setparam
define_syscall0 (__NR_sched_setparam, sched_setparam, linux_sched_setparam, void)
#endif
#ifdef __NR_sched_getparam
define_syscall0 (__NR_sched_getparam, sched_getparam, linux_sched_getparam, void)
#endif
#ifdef __NR_sched_setscheduler
define_syscall0 (__NR_sched_setscheduler, sched_setscheduler, linux_sched_setscheduler, void)
#endif
#ifdef __NR_sched_getscheduler
define_syscall0 (__NR_sched_getscheduler, sched_getscheduler, linux_sched_getscheduler, void)
#endif
#ifdef __NR_sched_get_priority_max
define_syscall0 (__NR_sched_get_priority_max, sched_get_priority_max, linux_sched_get_priority_max, void)
#endif
#ifdef __NR_sched_get_priority_min
define_syscall0 (__NR_sched_get_priority_min, sched_get_priority_min, linux_sched_get_priority_min, void)
#endif
#ifdef __NR_sched_rr_get_interval
define_syscall0 (__NR_sched_rr_get_interval, sched_rr_get_interval, linux_sched_rr_get_interval, void)
#endif
#ifdef __NR_mlock
define_syscall0 (__NR_mlock, mlock, linux_mlock, void)
#endif
#ifdef __NR_munlock
define_syscall0 (__NR_munlock, munlock, linux_munlock, void)
#endif
#ifdef __NR_mlockall
define_syscall0 (__NR_mlockall, mlockall, linux_mlockall, void)
#endif
#ifdef __NR_munlockall
define_syscall0 (__NR_munlockall, munlockall, linux_munlockall, void)
#endif
#ifdef __NR_vhangup
define_syscall0 (__NR_vhangup, vhangup, linux_vhangup, void)
#endif
#ifdef __NR_modify_ldt
define_syscall0 (__NR_modify_ldt, modify_ldt, linux_modify_ldt, void)
#endif
#ifdef __NR_pivot_root
define_syscall0 (__NR_pivot_root, pivot_root, linux_pivot_root, void)
#endif
#ifdef __NR__sysctl
define_syscall0 (__NR__sysctl, _sysctl, linux__sysctl, void)
#endif
#ifdef __NR_prctl
define_syscall0 (__NR_prctl, prctl, linux_prctl, void)
#endif
#ifdef __NR_arch_prctl
define_syscall0 (__NR_arch_prctl, arch_prctl, linux_arch_prctl, void)
#endif
#ifdef __NR_adjtimex
define_syscall0 (__NR_adjtimex, adjtimex, linux_adjtimex, void)
#endif
#ifdef __NR_setrlimit
define_syscall0 (__NR_setrlimit, setrlimit, linux_setrlimit, void)
#endif
#ifdef __NR_chroot
define_syscall0 (__NR_chroot, chroot, linux_chroot, void)
#endif
#ifdef __NR_sync
define_syscall0 (__NR_sync, sync, linux_sync, void)
#endif
#ifdef __NR_acct
define_syscall0 (__NR_acct, acct, linux_acct, void)
#endif
#ifdef __NR_settimeofday
define_syscall0 (__NR_settimeofday, settimeofday, linux_settimeofday, void)
#endif
#ifdef __NR_mount
define_syscall0 (__NR_mount, mount, linux_mount, void)
#endif
#ifdef __NR_umount
define_syscall0 (__NR_umount, umount, linux_umount, void)
#endif
#ifdef __NR_swapon
define_syscall0 (__NR_swapon, swapon, linux_swapon, void)
#endif
#ifdef __NR_swapoff
define_syscall0 (__NR_swapoff, swapoff, linux_swapoff, void)
#endif
#ifdef __NR_reboot
define_syscall0 (__NR_reboot, reboot, linux_reboot, void)
#endif
#ifdef __NR_sethostname
define_syscall0 (__NR_sethostname, sethostname, linux_sethostname, void)
#endif
#ifdef __NR_setdomainname
define_syscall0 (__NR_setdomainname, setdomainname, linux_setdomainname, void)
#endif
#ifdef __NR_iopl
define_syscall0 (__NR_iopl, iopl, linux_iopl, void)
#endif
#ifdef __NR_ioperm
define_syscall0 (__NR_ioperm, ioperm, linux_ioperm, void)
#endif
#ifdef __NR_create_module
define_syscall0 (__NR_create_module, create_module, linux_create_module, void)
#endif
#ifdef __NR_init_module
define_syscall0 (__NR_init_module, init_module, linux_init_module, void)
#endif
#ifdef __NR_delete_module
define_syscall0 (__NR_delete_module, delete_module, linux_delete_module, void)
#endif
#ifdef __NR_get_kernel_syms
define_syscall0 (__NR_get_kernel_syms, get_kernel_syms, linux_get_kernel_syms, void)
#endif
#ifdef __NR_query_module
define_syscall0 (__NR_query_module, query_module, linux_query_module, void)
#endif
#ifdef __NR_quotactl
define_syscall0 (__NR_quotactl, quotactl, linux_quotactl, void)
#endif
#ifdef __NR_nfsservctl
define_syscall0 (__NR_nfsservctl, nfsservctl, linux_nfsservctl, void)
#endif
#ifdef __NR_getpmsg
define_syscall0 (__NR_getpmsg, getpmsg, linux_getpmsg, void)
#endif
#ifdef __NR_putpmsg
define_syscall0 (__NR_putpmsg, putpmsg, linux_putpmsg, void)
#endif
#ifdef __NR_afs_syscall
define_syscall0 (__NR_afs_syscall, afs_syscall, linux_afs_syscall, void)
#endif
#ifdef __NR_tuxcall
define_syscall0 (__NR_tuxcall, tuxcall, linux_tuxcall, void)
#endif
#ifdef __NR_security
define_syscall0 (__NR_security, security, linux_security, void)
#endif
#ifdef __NR_gettid
define_syscall0 (__NR_gettid, gettid, linux_gettid, void)
#endif
#ifdef __NR_readahead
define_syscall0 (__NR_readahead, readahead, linux_readahead, void)
#endif
#ifdef __NR_setxattr
define_syscall0 (__NR_setxattr, setxattr, linux_setxattr, void)
#endif
#ifdef __NR_lsetxattr
define_syscall0 (__NR_lsetxattr, lsetxattr, linux_lsetxattr, void)
#endif
#ifdef __NR_fsetxattr
define_syscall0 (__NR_fsetxattr, fsetxattr, linux_fsetxattr, void)
#endif
#ifdef __NR_getxattr
define_syscall0 (__NR_getxattr, getxattr, linux_getxattr, void)
#endif
#ifdef __NR_lgetxattr
define_syscall0 (__NR_lgetxattr, lgetxattr, linux_lgetxattr, void)
#endif
#ifdef __NR_fgetxattr
define_syscall0 (__NR_fgetxattr, fgetxattr, linux_fgetxattr, void)
#endif
#ifdef __NR_listxattr
define_syscall0 (__NR_listxattr, listxattr, linux_listxattr, void)
#endif
#ifdef __NR_llistxattr
define_syscall0 (__NR_llistxattr, llistxattr, linux_llistxattr, void)
#endif
#ifdef __NR_flistxattr
define_syscall0 (__NR_flistxattr, flistxattr, linux_flistxattr, void)
#endif
#ifdef __NR_removexattr
define_syscall0 (__NR_removexattr, removexattr, linux_removexattr, void)
#endif
#ifdef __NR_lremovexattr
define_syscall0 (__NR_lremovexattr, lremovexattr, linux_lremovexattr, void)
#endif
#ifdef __NR_fremovexattr
define_syscall0 (__NR_fremovexattr, fremovexattr, linux_fremovexattr, void)
#endif
#ifdef __NR_tkill
define_syscall0 (__NR_tkill, tkill, linux_tkill, void)
#endif
#ifdef __NR_time
define_syscall0 (__NR_time, time, linux_time, void)
#endif
#ifdef __NR_futex
define_syscall0 (__NR_futex, futex, linux_futex, void)
#endif
#ifdef __NR_sched_setaffinity
define_syscall0 (__NR_sched_setaffinity, sched_setaffinity, linux_sched_setaffinity, void)
#endif
#ifdef __NR_sched_getaffinity
define_syscall0 (__NR_sched_getaffinity, sched_getaffinity, linux_sched_getaffinity, void)
#endif
#ifdef __NR_set_thread_area
define_syscall0 (__NR_set_thread_area, set_thread_area, linux_set_thread_area, void)
#endif
#ifdef __NR_io_setup
define_syscall0 (__NR_io_setup, io_setup, linux_io_setup, void)
#endif
#ifdef __NR_io_destroy
define_syscall0 (__NR_io_destroy, io_destroy, linux_io_destroy, void)
#endif
#ifdef __NR_io_getevents
define_syscall0 (__NR_io_getevents, io_getevents, linux_io_getevents, void)
#endif
#ifdef __NR_io_submit
define_syscall0 (__NR_io_submit, io_submit, linux_io_submit, void)
#endif
#ifdef __NR_io_cancel
define_syscall0 (__NR_io_cancel, io_cancel, linux_io_cancel, void)
#endif
#ifdef __NR_get_thread_area
define_syscall0 (__NR_get_thread_area, get_thread_area, linux_get_thread_area, void)
#endif
#ifdef __NR_lookup_dcookie
define_syscall0 (__NR_lookup_dcookie, lookup_dcookie, linux_lookup_dcookie, void)
#endif
#ifdef __NR_epoll_create
define_syscall0 (__NR_epoll_create, epoll_create, linux_epoll_create, void)
#endif
#ifdef __NR_epoll_ctl_old
define_syscall0 (__NR_epoll_ctl_old, epoll_ctl_old, linux_epoll_ctl_old, void)
#endif
#ifdef __NR_epoll_wait_old
define_syscall0 (__NR_epoll_wait_old, epoll_wait_old, linux_epoll_wait_old, void)
#endif
#ifdef __NR_remap_file_pages
define_syscall0 (__NR_remap_file_pages, remap_file_pages, linux_remap_file_pages, void)
#endif
#ifdef __NR_getdents64
define_syscall0 (__NR_getdents64, getdents64, linux_getdents64, void)
#endif
#ifdef __NR_set_tid_address
define_syscall0 (__NR_set_tid_address, set_tid_address, linux_set_tid_address, void)
#endif
#ifdef __NR_restart_syscall
define_syscall0 (__NR_restart_syscall, restart_syscall, linux_restart_syscall, void)
#endif
#ifdef __NR_semtimedop
define_syscall0 (__NR_semtimedop, semtimedop, linux_semtimedop, void)
#endif
#ifdef __NR_fadvise
define_syscall0 (__NR_fadvise, fadvise, linux_fadvise, void)
#endif
#ifdef __NR_timer_create
define_syscall0 (__NR_timer_create, timer_create, linux_timer_create, void)
#endif
#ifdef __NR_timer_settime
define_syscall0 (__NR_timer_settime, timer_settime, linux_timer_settime, void)
#endif
#ifdef __NR_timer_gettime
define_syscall0 (__NR_timer_gettime, timer_gettime, linux_timer_gettime, void)
#endif
#ifdef __NR_timer_getoverrun
define_syscall0 (__NR_timer_getoverrun, timer_getoverrun, linux_timer_getoverrun, void)
#endif
#ifdef __NR_timer_delete
define_syscall0 (__NR_timer_delete, timer_delete, linux_timer_delete, void)
#endif
#ifdef __NR_clock_settime
define_syscall0 (__NR_clock_settime, clock_settime, linux_clock_settime, void)
#endif
#ifdef __NR_clock_gettime
define_syscall0 (__NR_clock_gettime, clock_gettime, linux_clock_gettime, void)
#endif
#ifdef __NR_clock_getres
define_syscall0 (__NR_clock_getres, clock_getres, linux_clock_getres, void)
#endif
#ifdef __NR_clock_nanosleep
define_syscall0 (__NR_clock_nanosleep, clock_nanosleep, linux_clock_nanosleep, void)
#endif
#ifdef __NR_exit_group
define_syscall0 (__NR_exit_group, exit_group, linux_exit_group, void)
#endif
#ifdef __NR_epoll_wait
define_syscall0 (__NR_epoll_wait, epoll_wait, linux_epoll_wait, void)
#endif
#ifdef __NR_epoll_ctl
define_syscall0 (__NR_epoll_ctl, epoll_ctl, linux_epoll_ctl, void)
#endif
#ifdef __NR_tgkill
define_syscall0 (__NR_tgkill, tgkill, linux_tgkill, void)
#endif
#ifdef __NR_utimes
define_syscall0 (__NR_utimes, utimes, linux_utimes, void)
#endif
#ifdef __NR_vserver
define_syscall0 (__NR_vserver, vserver, linux_vserver, void)
#endif
#ifdef __NR_mbind
define_syscall0 (__NR_mbind, mbind, linux_mbind, void)
#endif
#ifdef __NR_set_mempolicy
define_syscall0 (__NR_set_mempolicy, set_mempolicy, linux_set_mempolicy, void)
#endif
#ifdef __NR_get_mempolicy
define_syscall0 (__NR_get_mempolicy, get_mempolicy, linux_get_mempolicy, void)
#endif
#ifdef __NR_mq_open
define_syscall0 (__NR_mq_open, mq_open, linux_mq_open, void)
#endif
#ifdef __NR_mq_unlink
define_syscall0 (__NR_mq_unlink, mq_unlink, linux_mq_unlink, void)
#endif
#ifdef __NR_mq_timedsend
define_syscall0 (__NR_mq_timedsend, mq_timedsend, linux_mq_timedsend, void)
#endif
#ifdef __NR_mq_timedreceive
define_syscall0 (__NR_mq_timedreceive, mq_timedreceive, linux_mq_timedreceive, void)
#endif
#ifdef __NR_mq_notify
define_syscall0 (__NR_mq_notify, mq_notify, linux_mq_notify, void)
#endif
#ifdef __NR_mq_getsetattr
define_syscall0 (__NR_mq_getsetattr, mq_getsetattr, linux_mq_getsetattr, void)
#endif
#ifdef __NR_kexec_load
define_syscall0 (__NR_kexec_load, kexec_load, linux_kexec_load, void)
#endif
#ifdef __NR_waitid
define_syscall0 (__NR_waitid, waitid, linux_waitid, void)
#endif
#ifdef __NR_add_key
define_syscall0 (__NR_add_key, add_key, linux_add_key, void)
#endif
#ifdef __NR_request_key
define_syscall0 (__NR_request_key, request_key, linux_request_key, void)
#endif
#ifdef __NR_keyctl
define_syscall0 (__NR_keyctl, keyctl, linux_keyctl, void)
#endif
#ifdef __NR_ioprio_set
define_syscall0 (__NR_ioprio_set, ioprio_set, linux_ioprio_set, void)
#endif
#ifdef __NR_ioprio_get
define_syscall0 (__NR_ioprio_get, ioprio_get, linux_ioprio_get, void)
#endif
#ifdef __NR_inotify_init
define_syscall0 (__NR_inotify_init, inotify_init, linux_inotify_init, void)
#endif
#ifdef __NR_inotify_add_watch
define_syscall0 (__NR_inotify_add_watch, inotify_add_watch, linux_inotify_add_watch, void)
#endif
#ifdef __NR_inotify_rm_watch
define_syscall0 (__NR_inotify_rm_watch, inotify_rm_watch, linux_inotify_rm_watch, void)
#endif
#ifdef __NR_migrate_pages
define_syscall0 (__NR_migrate_pages, migrate_pages, linux_migrate_pages, void)
#endif
#ifdef __NR_openat
define_syscall0 (__NR_openat, openat, linux_openat, void)
#endif
#ifdef __NR_mkdirat
define_syscall0 (__NR_mkdirat, mkdirat, linux_mkdirat, void)
#endif
#ifdef __NR_mknodat
define_syscall0 (__NR_mknodat, mknodat, linux_mknodat, void)
#endif
#ifdef __NR_fchownat
define_syscall0 (__NR_fchownat, fchownat, linux_fchownat, void)
#endif
#ifdef __NR_futimesat
define_syscall0 (__NR_futimesat, futimesat, linux_futimesat, void)
#endif
#ifdef __NR_newfstatat
define_syscall0 (__NR_newfstatat, newfstatat, linux_newfstatat, void)
#endif
#ifdef __NR_unlinkat
define_syscall0 (__NR_unlinkat, unlinkat, linux_unlinkat, void)
#endif
#ifdef __NR_renameat
define_syscall0 (__NR_renameat, renameat, linux_renameat, void)
#endif
#ifdef __NR_linkat
define_syscall0 (__NR_linkat, linkat, linux_linkat, void)
#endif
#ifdef __NR_symlinkat
define_syscall0 (__NR_symlinkat, symlinkat, linux_symlinkat, void)
#endif
#ifdef __NR_readlinkat
define_syscall0 (__NR_readlinkat, readlinkat, linux_readlinkat, void)
#endif
#ifdef __NR_fchmodat
define_syscall0 (__NR_fchmodat, fchmodat, linux_fchmodat, void)
#endif
#ifdef __NR_faccessat
define_syscall0 (__NR_faccessat, faccessat, linux_faccessat, void)
#endif
#ifdef __NR_pselect
define_syscall0 (__NR_pselect, pselect, linux_pselect, void)
#endif
#ifdef __NR_ppoll
define_syscall0 (__NR_ppoll, ppoll, linux_ppoll, void)
#endif
#ifdef __NR_unshare
define_syscall0 (__NR_unshare, unshare, linux_unshare, void)
#endif
#ifdef __NR_set_robust_list
define_syscall0 (__NR_set_robust_list, set_robust_list, linux_set_robust_list, void)
#endif
#ifdef __NR_get_robust_list
define_syscall0 (__NR_get_robust_list, get_robust_list, linux_get_robust_list, void)
#endif
#ifdef __NR_splice
define_syscall0 (__NR_splice, splice, linux_splice, void)
#endif
#ifdef __NR_tee
define_syscall0 (__NR_tee, tee, linux_tee, void)
#endif
#ifdef __NR_sync_file_range
define_syscall0 (__NR_sync_file_range, sync_file_range, linux_sync_file_range, void)
#endif
#ifdef __NR_vmsplice
define_syscall0 (__NR_vmsplice, vmsplice, linux_vmsplice, void)
#endif
#ifdef __NR_move_pages
define_syscall0 (__NR_move_pages, move_pages, linux_move_pages, void)
#endif
#ifdef __NR_utimensat
define_syscall0 (__NR_utimensat, utimensat, linux_utimensat, void)
#endif
#ifdef __NR_epoll_pwait
define_syscall0 (__NR_epoll_pwait, epoll_pwait, linux_epoll_pwait, void)
#endif
#ifdef __NR_signalfd
define_syscall0 (__NR_signalfd, signalfd, linux_signalfd, void)
#endif
#ifdef __NR_timerfd
define_syscall0 (__NR_timerfd, timerfd, linux_timerfd, void)
#endif
#ifdef __NR_eventfd
define_syscall0 (__NR_eventfd, eventfd, linux_eventfd, void)
#endif
#ifdef __NR_fallocate
define_syscall0 (__NR_fallocate, fallocate, linux_fallocate, void)
#endif

#endif
