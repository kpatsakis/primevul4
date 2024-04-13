proto_register_sysdig_event(void)
{
    /* XXX Match up with Sysdig's names. */
    static hf_register_info hf[] = {
        { &hf_se_cpu_id,
          { "CPU ID", "sysdig.cpu_id",
            FT_UINT16, BASE_DEC, NULL, 0, NULL, HFILL }
        },
        { &hf_se_thread_id,
          { "Thread ID", "sysdig.thread_id",
            FT_UINT64, BASE_DEC, NULL, 0, NULL, HFILL }
        },
        { &hf_se_event_length,
          { "Event length", "sysdig.event_len",
            FT_UINT32, BASE_DEC, NULL, 0, NULL, HFILL }
        },
        { &hf_se_nparams,
          { "Number of parameters", "sysdig.nparams",
            FT_UINT32, BASE_DEC, NULL, 0, NULL, HFILL }
        },
        { &hf_se_event_type,
          { "Event type", "sysdig.event_type",
            FT_UINT16, BASE_DEC, VALS(event_type_vals), 0, NULL, HFILL }
        },
        { &hf_se_param_lens,
          { "Parameter lengths", "sysdig.param.lens",
            FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL }
        },
        { &hf_se_param_len,
          { "Parameter length", "sysdig.param.len",
            FT_UINT16, BASE_DEC, NULL, 0, NULL, HFILL }
        },

/* Header field registration. Automatically generated by tools/generate-sysdig-event.py */
        { &hf_param_ID_bytes, { "ID", "sysdig.param.syscall.ID", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_action_uint32, { "action", "sysdig.param.cpu_hotplug.action", FT_UINT32, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_addr_bytes, { "addr", "sysdig.param.ptrace.addr", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_addr_uint64, { "addr", "sysdig.param.page_fault.addr", FT_UINT64, BASE_HEX, NULL, 0, NULL, HFILL } },
        { &hf_param_args_string, { "Program arguments", "sysdig.param.execve.args", FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_argument_uint64, { "I/O control: argument", "sysdig.param.ioctl.argument", FT_UINT64, BASE_HEX, NULL, 0, NULL, HFILL } },
        { &hf_param_backlog_uint32, { "backlog", "sysdig.param.listen.backlog", FT_UINT32, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_cgroups_bytes, { "cgroups", "sysdig.param.execve.cgroups", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_clockid_uint8, { "clockid", "sysdig.param.timerfd_create.clockid", FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_cmd_bytes, { "cmd", "sysdig.param.semctl.cmd", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_cmd_int64, { "cmd", "sysdig.param.bpf.cmd", FT_INT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_comm_string, { "Command", "sysdig.param.execve.comm", FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_core_uint8, { "core", "sysdig.param.procexit.core", FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_cpu_sys_uint64, { "cpu_sys", "sysdig.param.procinfo.cpu_sys", FT_UINT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_cpu_uint32, { "cpu", "sysdig.param.cpu_hotplug.cpu", FT_UINT32, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_cpu_usr_uint64, { "cpu_usr", "sysdig.param.procinfo.cpu_usr", FT_UINT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_cur_int64, { "cur", "sysdig.param.setrlimit.cur", FT_INT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_cwd_string, { "Current working directory", "sysdig.param.execve.cwd", FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_data_bytes, { "data", "sysdig.param.ptrace.data", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_desc_string, { "desc", "sysdig.param.notification.desc", FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_description_string, { "description", "sysdig.param.infra.description", FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_dev_string, { "dev", "sysdig.param.mount.dev", FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_dev_uint32, { "dev", "sysdig.param.openat.dev", FT_UINT32, BASE_HEX, NULL, 0, NULL, HFILL } },
        { &hf_param_dir_string, { "dir", "sysdig.param.mount.dir", FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_dirfd_int64, { "dirfd", "sysdig.param.openat2.dirfd", FT_INT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_domain_bytes, { "domain", "sysdig.param.socketpair.domain", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_dpid_bytes, { "dpid", "sysdig.param.signaldeliver.dpid", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_dqb_bhardlimit_uint64, { "dqb_bhardlimit", "sysdig.param.quotactl.dqb_bhardlimit", FT_UINT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_dqb_bsoftlimit_uint64, { "dqb_bsoftlimit", "sysdig.param.quotactl.dqb_bsoftlimit", FT_UINT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_dqb_btime_bytes, { "dqb_btime", "sysdig.param.quotactl.dqb_btime", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_dqb_curspace_uint64, { "dqb_curspace", "sysdig.param.quotactl.dqb_curspace", FT_UINT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_dqb_ihardlimit_uint64, { "dqb_ihardlimit", "sysdig.param.quotactl.dqb_ihardlimit", FT_UINT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_dqb_isoftlimit_uint64, { "dqb_isoftlimit", "sysdig.param.quotactl.dqb_isoftlimit", FT_UINT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_dqb_itime_bytes, { "dqb_itime", "sysdig.param.quotactl.dqb_itime", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_dqi_bgrace_bytes, { "dqi_bgrace", "sysdig.param.quotactl.dqi_bgrace", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_dqi_flags_bytes, { "dqi_flags", "sysdig.param.quotactl.dqi_flags", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_dqi_igrace_bytes, { "dqi_igrace", "sysdig.param.quotactl.dqi_igrace", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_egid_bytes, { "egid", "sysdig.param.getresgid.egid", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_env_string, { "env", "sysdig.param.execve.env", FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_error_bytes, { "error", "sysdig.param.page_fault.error", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_euid_bytes, { "euid", "sysdig.param.getresuid.euid", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_event_data_bytes, { "event_data", "sysdig.param.pluginevent.event_data", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_event_data_uint64, { "event_data", "sysdig.param.sysdigevent.event_data", FT_UINT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_event_type_uint32, { "event_type", "sysdig.param.sysdigevent.event_type", FT_UINT32, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_exe_string, { "exe", "sysdig.param.execve.exe", FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_fd1_int64, { "fd1", "sysdig.param.pipe.fd1", FT_INT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_fd2_int64, { "fd2", "sysdig.param.pipe.fd2", FT_INT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_fd_in_int64, { "fd_in", "sysdig.param.splice.fd_in", FT_INT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_fd_int64, { "fd", "sysdig.param.openat2.fd", FT_INT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_fd_out_int64, { "fd_out", "sysdig.param.splice.fd_out", FT_INT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_fdlimit_int64, { "fdlimit", "sysdig.param.vfork.fdlimit", FT_INT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_fdlimit_uint64, { "fdlimit", "sysdig.param.execve.fdlimit", FT_UINT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_fds_bytes, { "fds", "sysdig.param.ppoll.fds", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_filename_bytes, { "filename", "sysdig.param.fchmodat.filename", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_filename_string, { "filename", "sysdig.param.chmod.filename", FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_flags_bytes, { "flags", "sysdig.param.openat2.flags", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_flags_uint32, { "flags", "sysdig.param.accept.flags", FT_UINT32, BASE_HEX, NULL, 0, NULL, HFILL } },
        { &hf_param_gid_bytes, { "gid", "sysdig.param.getgid.gid", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_gid_uint32, { "gid", "sysdig.param.vfork.gid", FT_UINT32, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_how_bytes, { "how", "sysdig.param.shutdown.how", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_id_int64, { "id", "sysdig.param.tracer.id", FT_INT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_id_string, { "id", "sysdig.param.notification.id", FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_id_uint32, { "id", "sysdig.param.quotactl.id", FT_UINT32, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_image_string, { "image", "sysdig.param.container.image", FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_in_fd_int64, { "in_fd", "sysdig.param.sendfile.in_fd", FT_INT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_initval_uint64, { "initval", "sysdig.param.eventfd.initval", FT_UINT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_ino_uint64, { "ino", "sysdig.param.pipe.ino", FT_UINT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_interval_bytes, { "interval", "sysdig.param.nanosleep.interval", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_ip_uint64, { "ip", "sysdig.param.page_fault.ip", FT_UINT64, BASE_HEX, NULL, 0, NULL, HFILL } },
        { &hf_param_json_string, { "json", "sysdig.param.container.json", FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_key_int32, { "key", "sysdig.param.semget.key", FT_INT32, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_length_uint64, { "length", "sysdig.param.munmap.length", FT_UINT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_level_bytes, { "level", "sysdig.param.getsockopt.level", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_linkdirfd_int64, { "linkdirfd", "sysdig.param.symlinkat.linkdirfd", FT_INT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_linkpath_bytes, { "linkpath", "sysdig.param.symlinkat.linkpath", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_linkpath_string, { "linkpath", "sysdig.param.symlink.linkpath", FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_loginuid_int32, { "loginuid", "sysdig.param.execve.loginuid", FT_INT32, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_mask_uint32, { "mask", "sysdig.param.signalfd.mask", FT_UINT32, BASE_HEX, NULL, 0, NULL, HFILL } },
        { &hf_param_max_int64, { "max", "sysdig.param.setrlimit.max", FT_INT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_maxevents_bytes, { "maxevents", "sysdig.param.epoll_wait.maxevents", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_mode_bytes, { "mode", "sysdig.param.fchmod.mode", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_mode_uint32, { "mode", "sysdig.param.openat2.mode", FT_UINT32, BASE_OCT, NULL, 0, NULL, HFILL } },
        { &hf_param_name_bytes, { "name", "sysdig.param.openat2.name", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_name_string, { "name", "sysdig.param.infra.name", FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_nativeID_uint16, { "nativeID", "sysdig.param.syscall.nativeID", FT_UINT16, BASE_DEC, VALS(nativeID_uint16_vals), 0, NULL, HFILL } },
        { &hf_param_newcur_int64, { "newcur", "sysdig.param.prlimit.newcur", FT_INT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_newdir_int64, { "newdir", "sysdig.param.linkat.newdir", FT_INT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_newdirfd_int64, { "newdirfd", "sysdig.param.renameat2.newdirfd", FT_INT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_newmax_int64, { "newmax", "sysdig.param.prlimit.newmax", FT_INT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_newpath_bytes, { "newpath", "sysdig.param.renameat2.newpath", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_newpath_string, { "newpath", "sysdig.param.link.newpath", FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_next_bytes, { "next", "sysdig.param.switch.next", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_nsems_int32, { "nsems", "sysdig.param.semget.nsems", FT_INT32, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_nsops_uint32, { "nsops", "sysdig.param.semop.nsops", FT_UINT32, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_nstype_bytes, { "nstype", "sysdig.param.setns.nstype", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_offset_uint64, { "offset", "sysdig.param.sendfile.offset", FT_UINT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_oldcur_int64, { "oldcur", "sysdig.param.prlimit.oldcur", FT_INT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_olddir_int64, { "olddir", "sysdig.param.linkat.olddir", FT_INT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_olddirfd_int64, { "olddirfd", "sysdig.param.renameat2.olddirfd", FT_INT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_oldmax_int64, { "oldmax", "sysdig.param.prlimit.oldmax", FT_INT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_oldpath_bytes, { "oldpath", "sysdig.param.renameat2.oldpath", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_oldpath_string, { "oldpath", "sysdig.param.link.oldpath", FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_op_bytes, { "op", "sysdig.param.futex.op", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_op_uint64, { "op", "sysdig.param.seccomp.op", FT_UINT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_operation_bytes, { "operation", "sysdig.param.flock.operation", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_optlen_uint32, { "optlen", "sysdig.param.getsockopt.optlen", FT_UINT32, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_optname_bytes, { "optname", "sysdig.param.getsockopt.optname", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_out_fd_int64, { "out_fd", "sysdig.param.sendfile.out_fd", FT_INT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_path_bytes, { "path", "sysdig.param.mkdirat.path", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_path_string, { "path", "sysdig.param.unlink.path", FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_peer_uint64, { "peer", "sysdig.param.socketpair.peer", FT_UINT64, BASE_HEX, NULL, 0, NULL, HFILL } },
        { &hf_param_pgft_maj_uint64, { "pgft_maj", "sysdig.param.execve.pgft_maj", FT_UINT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_pgft_min_uint64, { "pgft_min", "sysdig.param.execve.pgft_min", FT_UINT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_pgid_bytes, { "pgid", "sysdig.param.setpgid.pgid", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_pgoffset_uint64, { "pgoffset", "sysdig.param.mmap2.pgoffset", FT_UINT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_pid_bytes, { "pid", "sysdig.param.setpgid.pid", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_plugin_ID_uint32, { "plugin_ID", "sysdig.param.pluginevent.plugin_ID", FT_UINT32, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_pos_uint64, { "pos", "sysdig.param.pwritev.pos", FT_UINT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_prot_bytes, { "prot", "sysdig.param.mmap2.prot", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_proto_uint32, { "proto", "sysdig.param.socketpair.proto", FT_UINT32, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_ptid_bytes, { "ptid", "sysdig.param.execve.ptid", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_queuelen_uint32, { "queuelen", "sysdig.param.accept.queuelen", FT_UINT32, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_queuemax_uint32, { "queuemax", "sysdig.param.accept.queuemax", FT_UINT32, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_queuepct_uint8, { "Accept queue per connection", "sysdig.param.accept.queuepct", FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_quota_fmt_bytes, { "quota_fmt", "sysdig.param.quotactl.quota_fmt", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_quota_fmt_out_bytes, { "quota_fmt_out", "sysdig.param.quotactl.quota_fmt_out", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_quotafilepath_string, { "quotafilepath", "sysdig.param.quotactl.quotafilepath", FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_ratio_uint32, { "ratio", "sysdig.param.drop.ratio", FT_UINT32, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_request_bytes, { "request", "sysdig.param.ptrace.request", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_request_uint64, { "I/O control: request", "sysdig.param.ioctl.request", FT_UINT64, BASE_HEX, NULL, 0, NULL, HFILL } },
        { &hf_param_res_bytes, { "res", "sysdig.param.userfaultfd.res", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_res_int64, { "res", "sysdig.param.fcntl.res", FT_INT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_res_or_fd_bytes, { "res_or_fd", "sysdig.param.bpf.res_or_fd", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_res_uint64, { "res", "sysdig.param.mmap2.res", FT_UINT64, BASE_HEX, NULL, 0, NULL, HFILL } },
        { &hf_param_resolve_bytes, { "resolve", "sysdig.param.openat2.resolve", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_resource_bytes, { "resource", "sysdig.param.prlimit.resource", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_ret_bytes, { "ret", "sysdig.param.procexit.ret", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_rgid_bytes, { "rgid", "sysdig.param.getresgid.rgid", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_ruid_bytes, { "ruid", "sysdig.param.getresuid.ruid", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_scope_string, { "scope", "sysdig.param.infra.scope", FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_sem_flg_0_bytes, { "sem_flg_0", "sysdig.param.semop.sem_flg_0", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_sem_flg_1_bytes, { "sem_flg_1", "sysdig.param.semop.sem_flg_1", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_sem_num_0_uint16, { "sem_num_0", "sysdig.param.semop.sem_num_0", FT_UINT16, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_sem_num_1_uint16, { "sem_num_1", "sysdig.param.semop.sem_num_1", FT_UINT16, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_sem_op_0_int16, { "sem_op_0", "sysdig.param.semop.sem_op_0", FT_INT16, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_sem_op_1_int16, { "sem_op_1", "sysdig.param.semop.sem_op_1", FT_INT16, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_semflg_bytes, { "semflg", "sysdig.param.semget.semflg", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_semid_int32, { "semid", "sysdig.param.semctl.semid", FT_INT32, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_semnum_int32, { "semnum", "sysdig.param.semctl.semnum", FT_INT32, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_sgid_bytes, { "sgid", "sysdig.param.getresgid.sgid", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_sig_bytes, { "sig", "sysdig.param.signaldeliver.sig", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_sigmask_bytes, { "sigmask", "sysdig.param.ppoll.sigmask", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_size_uint32, { "size", "sysdig.param.pwritev.size", FT_UINT32, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_size_uint64, { "size", "sysdig.param.sendfile.size", FT_UINT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_source_string, { "source", "sysdig.param.infra.source", FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_source_uint64, { "source", "sysdig.param.socketpair.source", FT_UINT64, BASE_HEX, NULL, 0, NULL, HFILL } },
        { &hf_param_special_string, { "special", "sysdig.param.quotactl.special", FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_spid_bytes, { "spid", "sysdig.param.signaldeliver.spid", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_status_bytes, { "status", "sysdig.param.procexit.status", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_suid_bytes, { "suid", "sysdig.param.getresuid.suid", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_tags_bytes, { "tags", "sysdig.param.tracer.tags", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_target_string, { "target", "sysdig.param.symlinkat.target", FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_tid_bytes, { "tid", "sysdig.param.execve.tid", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_timeout_bytes, { "timeout", "sysdig.param.ppoll.timeout", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_timeout_int64, { "timeout", "sysdig.param.poll.timeout", FT_INT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_tty_int32, { "tty", "sysdig.param.execve.tty", FT_INT32, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_tuple_bytes, { "tuple", "sysdig.param.accept.tuple", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_type_bytes, { "type", "sysdig.param.quotactl.type", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_type_string, { "type", "sysdig.param.mount.type", FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_type_uint32, { "type", "sysdig.param.container.type", FT_UINT32, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_uid_bytes, { "uid", "sysdig.param.getuid.uid", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_uid_uint32, { "uid", "sysdig.param.vfork.uid", FT_UINT32, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_val_bytes, { "val", "sysdig.param.getsockopt.val", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_val_int32, { "val", "sysdig.param.semctl.val", FT_INT32, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_val_uint64, { "val", "sysdig.param.futex.val", FT_UINT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_vm_rss_uint32, { "vm_rss", "sysdig.param.execve.vm_rss", FT_UINT32, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_vm_size_uint32, { "vm_size", "sysdig.param.execve.vm_size", FT_UINT32, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_vm_swap_uint32, { "vm_swap", "sysdig.param.execve.vm_swap", FT_UINT32, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_param_vpid_bytes, { "vpid", "sysdig.param.vfork.vpid", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_vtid_bytes, { "vtid", "sysdig.param.vfork.vtid", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_param_whence_bytes, { "whence", "sysdig.param.llseek.whence", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL } },
    };

    /* Setup protocol subtree array */
    static gint *ett[] = {
        &ett_sysdig_event,
        &ett_sysdig_parm_lens,
        &ett_sysdig_syscall
    };

    /* Register the protocol name and description */
    proto_sysdig_event = proto_register_protocol("Sysdig System Call",
            "Sysdig Event", "sysdig");

    /* Required function calls to register the header fields and subtrees */
    proto_register_field_array(proto_sysdig_event, hf, array_length(hf));
    proto_register_subtree_array(ett, array_length(ett));

    register_dissector("sysdig", dissect_sysdig_event, proto_sysdig_event);
}