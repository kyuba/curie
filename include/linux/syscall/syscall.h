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

#ifdef __NR_read
#endif
#ifdef __NR_write
#endif
#ifdef __NR_open
#endif
#ifdef __NR_close
#endif
#ifdef __NR_stat
#endif
#ifdef __NR_fstat
#endif
#ifdef __NR_lstat
#endif
#ifdef __NR_poll
#endif
#ifdef __NR_lseek
#endif
#ifdef __NR_mmap
#endif
#ifdef __NR_mprotect
#endif
#ifdef __NR_munmap
#endif
#ifdef __NR_brk
#endif
#ifdef __NR_rt_sigaction
#endif
#ifdef __NR_rt_sigprocmask
#endif
#ifdef __NR_rt_sigreturn
#endif
#ifdef __NR_ioctl
#endif
#ifdef __NR_pread
#endif
#ifdef __NR_pwrite
#endif
#ifdef __NR_readv
#endif
#ifdef __NR_writev
#endif
#ifdef __NR_access
#endif
#ifdef __NR_pipe
#endif
#ifdef __NR_select
#endif
#ifdef __NR_sched_yield
#endif
#ifdef __NR_mremap
#endif
#ifdef __NR_msync
#endif
#ifdef __NR_mincore
#endif
#ifdef __NR_madvise
#endif
#ifdef __NR_shmget
#endif
#ifdef __NR_shmat
#endif
#ifdef __NR_shmctl
#endif
#ifdef __NR_dup
#endif
#ifdef __NR_dup
#endif
#ifdef __NR_pause
#endif
#ifdef __NR_nanosleep
#endif
#ifdef __NR_getitimer
#endif
#ifdef __NR_alarm
#endif
#ifdef __NR_setitimer
#endif
#ifdef __NR_getpid
#endif
#ifdef __NR_sendfile
#endif
#ifdef __NR_socket
#endif
#ifdef __NR_connect
#endif
#ifdef __NR_accept
#endif
#ifdef __NR_sendto
#endif
#ifdef __NR_recvfrom
#endif
#ifdef __NR_sendmsg
#endif
#ifdef __NR_recvmsg
#endif
#ifdef __NR_shutdown
#endif
#ifdef __NR_bind
#endif
#ifdef __NR_listen
#endif
#ifdef __NR_getsockname
#endif
#ifdef __NR_getpeername
#endif
#ifdef __NR_socketpair
#endif
#ifdef __NR_setsockopt
#endif
#ifdef __NR_getsockopt
#endif
#ifdef __NR_clone
#endif
#ifdef __NR_fork
#endif
#ifdef __NR_vfork
#endif
#ifdef __NR_execve
#endif
#ifdef __NR_exit
#endif
#ifdef __NR_wait
#endif
#ifdef __NR_kill
#endif
#ifdef __NR_uname
#endif
#ifdef __NR_semget
#endif
#ifdef __NR_semop
#endif
#ifdef __NR_semctl
#endif
#ifdef __NR_shmdt
#endif
#ifdef __NR_msgget
#endif
#ifdef __NR_msgsnd
#endif
#ifdef __NR_msgrcv
#endif
#ifdef __NR_msgctl
#endif
#ifdef __NR_fcntl
#endif
#ifdef __NR_flock
#endif
#ifdef __NR_fsync
#endif
#ifdef __NR_fdatasync
#endif
#ifdef __NR_truncate
#endif
#ifdef __NR_ftruncate
#endif
#ifdef __NR_getdents
#endif
#ifdef __NR_getcwd
#endif
#ifdef __NR_chdir
#endif
#ifdef __NR_fchdir
#endif
#ifdef __NR_rename
#endif
#ifdef __NR_mkdir
#endif
#ifdef __NR_rmdir
#endif
#ifdef __NR_creat
#endif
#ifdef __NR_link
#endif
#ifdef __NR_unlink
#endif
#ifdef __NR_symlink
#endif
#ifdef __NR_readlink
#endif
#ifdef __NR_chmod
#endif
#ifdef __NR_fchmod
#endif
#ifdef __NR_chown
#endif
#ifdef __NR_fchown
#endif
#ifdef __NR_lchown
#endif
#ifdef __NR_umask
#endif
#ifdef __NR_gettimeofday
#endif
#ifdef __NR_getrlimit
#endif
#ifdef __NR_getrusage
#endif
#ifdef __NR_sysinfo
#endif
#ifdef __NR_times
#endif
#ifdef __NR_ptrace
#endif
#ifdef __NR_getuid
#endif
#ifdef __NR_syslog
#endif
#ifdef __NR_getgid
#endif
#ifdef __NR_setuid
#endif
#ifdef __NR_setgid
#endif
#ifdef __NR_geteuid
#endif
#ifdef __NR_getegid
#endif
#ifdef __NR_setpgid
#endif
#ifdef __NR_getppid
#endif
#ifdef __NR_getpgrp
#endif
#ifdef __NR_setsid
#endif
#ifdef __NR_setreuid
#endif
#ifdef __NR_setregid
#endif
#ifdef __NR_getgroups
#endif
#ifdef __NR_setgroups
#endif
#ifdef __NR_setresuid
#endif
#ifdef __NR_getresuid
#endif
#ifdef __NR_setresgid
#endif
#ifdef __NR_getresgid
#endif
#ifdef __NR_getpgid
#endif
#ifdef __NR_setfsuid
#endif
#ifdef __NR_setfsgid
#endif
#ifdef __NR_getsid
#endif
#ifdef __NR_capget
#endif
#ifdef __NR_capset
#endif
#ifdef __NR_rt_sigpending
#endif
#ifdef __NR_rt_sigtimedwait
#endif
#ifdef __NR_rt_sigqueueinfo
#endif
#ifdef __NR_rt_sigsuspend
#endif
#ifdef __NR_sigaltstack
#endif
#ifdef __NR_utime
#endif
#ifdef __NR_mknod
#endif
#ifdef __NR_uselib
#endif
#ifdef __NR_personality
#endif
#ifdef __NR_ustat
#endif
#ifdef __NR_statfs
#endif
#ifdef __NR_fstatfs
#endif
#ifdef __NR_sysfs
#endif
#ifdef __NR_getpriority
#endif
#ifdef __NR_setpriority
#endif
#ifdef __NR_sched_setparam
#endif
#ifdef __NR_sched_getparam
#endif
#ifdef __NR_sched_setscheduler
#endif
#ifdef __NR_sched_getscheduler
#endif
#ifdef __NR_sched_get_priority_max
#endif
#ifdef __NR_sched_get_priority_min
#endif
#ifdef __NR_sched_rr_get_interval
#endif
#ifdef __NR_mlock
#endif
#ifdef __NR_munlock
#endif
#ifdef __NR_mlockall
#endif
#ifdef __NR_munlockall
#endif
#ifdef __NR_vhangup
#endif
#ifdef __NR_modify_ldt
#endif
#ifdef __NR_pivot_root
#endif
#ifdef __NR__sysctl
#endif
#ifdef __NR_prctl
#endif
#ifdef __NR_arch_prctl
#endif
#ifdef __NR_adjtimex
#endif
#ifdef __NR_setrlimit
#endif
#ifdef __NR_chroot
#endif
#ifdef __NR_sync
#endif
#ifdef __NR_acct
#endif
#ifdef __NR_settimeofday
#endif
#ifdef __NR_mount
#endif
#ifdef __NR_umount
#endif
#ifdef __NR_swapon
#endif
#ifdef __NR_swapoff
#endif
#ifdef __NR_reboot
#endif
#ifdef __NR_sethostname
#endif
#ifdef __NR_setdomainname
#endif
#ifdef __NR_iopl
#endif
#ifdef __NR_ioperm
#endif
#ifdef __NR_create_module
#endif
#ifdef __NR_init_module
#endif
#ifdef __NR_delete_module
#endif
#ifdef __NR_get_kernel_syms
#endif
#ifdef __NR_query_module
#endif
#ifdef __NR_quotactl
#endif
#ifdef __NR_nfsservctl
#endif
#ifdef __NR_getpmsg
#endif
#ifdef __NR_putpmsg
#endif
#ifdef __NR_afs_syscall
#endif
#ifdef __NR_tuxcall
#endif
#ifdef __NR_security
#endif
#ifdef __NR_gettid
#endif
#ifdef __NR_readahead
#endif
#ifdef __NR_setxattr
#endif
#ifdef __NR_lsetxattr
#endif
#ifdef __NR_fsetxattr
#endif
#ifdef __NR_getxattr
#endif
#ifdef __NR_lgetxattr
#endif
#ifdef __NR_fgetxattr
#endif
#ifdef __NR_listxattr
#endif
#ifdef __NR_llistxattr
#endif
#ifdef __NR_flistxattr
#endif
#ifdef __NR_removexattr
#endif
#ifdef __NR_lremovexattr
#endif
#ifdef __NR_fremovexattr
#endif
#ifdef __NR_tkill
#endif
#ifdef __NR_time
#endif
#ifdef __NR_futex
#endif
#ifdef __NR_sched_setaffinity
#endif
#ifdef __NR_sched_getaffinity
#endif
#ifdef __NR_set_thread_area
#endif
#ifdef __NR_io_setup
#endif
#ifdef __NR_io_destroy
#endif
#ifdef __NR_io_getevents
#endif
#ifdef __NR_io_submit
#endif
#ifdef __NR_io_cancel
#endif
#ifdef __NR_get_thread_area
#endif
#ifdef __NR_lookup_dcookie
#endif
#ifdef __NR_epoll_create
#endif
#ifdef __NR_epoll_ctl_old
#endif
#ifdef __NR_epoll_wait_old
#endif
#ifdef __NR_remap_file_pages
#endif
#ifdef __NR_getdents
#endif
#ifdef __NR_set_tid_address
#endif
#ifdef __NR_restart_syscall
#endif
#ifdef __NR_semtimedop
#endif
#ifdef __NR_fadvise
#endif
#ifdef __NR_timer_create
#endif
#ifdef __NR_timer_settime
#endif
#ifdef __NR_timer_gettime
#endif
#ifdef __NR_timer_getoverrun
#endif
#ifdef __NR_timer_delete
#endif
#ifdef __NR_clock_settime
#endif
#ifdef __NR_clock_gettime
#endif
#ifdef __NR_clock_getres
#endif
#ifdef __NR_clock_nanosleep
#endif
#ifdef __NR_exit_group
#endif
#ifdef __NR_epoll_wait
#endif
#ifdef __NR_epoll_ctl
#endif
#ifdef __NR_tgkill
#endif
#ifdef __NR_utimes
#endif
#ifdef __NR_vserver
#endif
#ifdef __NR_mbind
#endif
#ifdef __NR_set_mempolicy
#endif
#ifdef __NR_get_mempolicy
#endif
#ifdef __NR_mq_open
#endif
#ifdef __NR_mq_unlink
#endif
#ifdef __NR_mq_timedsend
#endif
#ifdef __NR_mq_timedreceive
#endif
#ifdef __NR_mq_notify
#endif
#ifdef __NR_mq_getsetattr
#endif
#ifdef __NR_kexec_load
#endif
#ifdef __NR_waitid
#endif
#ifdef __NR_add_key
#endif
#ifdef __NR_request_key
#endif
#ifdef __NR_keyctl
#endif
#ifdef __NR_ioprio_set
#endif
#ifdef __NR_ioprio_get
#endif
#ifdef __NR_inotify_init
#endif
#ifdef __NR_inotify_add_watch
#endif
#ifdef __NR_inotify_rm_watch
#endif
#ifdef __NR_migrate_pages
#endif
#ifdef __NR_openat
#endif
#ifdef __NR_mkdirat
#endif
#ifdef __NR_mknodat
#endif
#ifdef __NR_fchownat
#endif
#ifdef __NR_futimesat
#endif
#ifdef __NR_newfstatat
#endif
#ifdef __NR_unlinkat
#endif
#ifdef __NR_renameat
#endif
#ifdef __NR_linkat
#endif
#ifdef __NR_symlinkat
#endif
#ifdef __NR_readlinkat
#endif
#ifdef __NR_fchmodat
#endif
#ifdef __NR_faccessat
#endif
#ifdef __NR_pselect
#endif
#ifdef __NR_ppoll
#endif
#ifdef __NR_unshare
#endif
#ifdef __NR_set_robust_list
#endif
#ifdef __NR_get_robust_list
#endif
#ifdef __NR_splice
#endif
#ifdef __NR_tee
#endif
#ifdef __NR_sync_file_range
#endif
#ifdef __NR_vmsplice
#endif
#ifdef __NR_move_pages
#endif
#ifdef __NR_utimensat
#endif
#ifdef __NR_epoll_pwait
#endif
#ifdef __NR_signalfd
#endif
#ifdef __NR_timerfd
#endif
#ifdef __NR_eventfd
#endif
#ifdef __NR_fallocate
#endif

#endif
