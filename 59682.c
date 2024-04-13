parse_args_recurse (int    *argcp,
                    char ***argvp,
                    bool    in_file,
                    int    *total_parsed_argc_p)
{
  SetupOp *op;
  int argc = *argcp;
  char **argv = *argvp;
  /* I can't imagine a case where someone wants more than this.
   * If you do...you should be able to pass multiple files
   * via a single tmpfs and linking them there, etc.
   *
   * We're adding this hardening due to precedent from
   * http://googleprojectzero.blogspot.com/2014/08/the-poisoned-nul-byte-2014-edition.html
   *
   * I picked 9000 because the Internet told me to and it was hard to
   * resist.
   */
  static const uint32_t MAX_ARGS = 9000;

  if (*total_parsed_argc_p > MAX_ARGS)
    die ("Exceeded maximum number of arguments %u", MAX_ARGS);

  while (argc > 0)
    {
      const char *arg = argv[0];

      if (strcmp (arg, "--help") == 0)
        {
          usage (EXIT_SUCCESS, stdout);
        }
      else if (strcmp (arg, "--version") == 0)
        {
          printf ("%s\n", PACKAGE_STRING);
          exit (0);
        }
      else if (strcmp (arg, "--args") == 0)
        {
          int the_fd;
          char *endptr;
          char *data, *p;
          char *data_end;
          size_t data_len;
          cleanup_free char **data_argv = NULL;
          char **data_argv_copy;
          int data_argc;
          int i;

          if (in_file)
            die ("--args not supported in arguments file");

          if (argc < 2)
            die ("--args takes an argument");

          the_fd = strtol (argv[1], &endptr, 10);
          if (argv[1][0] == 0 || endptr[0] != 0 || the_fd < 0)
            die ("Invalid fd: %s", argv[1]);

          data = load_file_data (the_fd, &data_len);
          if (data == NULL)
            die_with_error ("Can't read --args data");

          data_end = data + data_len;
          data_argc = 0;

          p = data;
          while (p != NULL && p < data_end)
            {
              data_argc++;
              (*total_parsed_argc_p)++;
              if (*total_parsed_argc_p > MAX_ARGS)
                die ("Exceeded maximum number of arguments %u", MAX_ARGS);
              p = memchr (p, 0, data_end - p);
              if (p != NULL)
                p++;
            }

          data_argv = xcalloc (sizeof (char *) * (data_argc + 1));

          i = 0;
          p = data;
          while (p != NULL && p < data_end)
            {
              /* Note: load_file_data always adds a nul terminator, so this is safe
               * even for the last string. */
              data_argv[i++] = p;
              p = memchr (p, 0, data_end - p);
              if (p != NULL)
                p++;
            }

          data_argv_copy = data_argv; /* Don't change data_argv, we need to free it */
          parse_args_recurse (&data_argc, &data_argv_copy, TRUE, total_parsed_argc_p);

          argv += 1;
          argc -= 1;
        }
      else if (strcmp (arg, "--unshare-user") == 0)
        {
          opt_unshare_user = TRUE;
        }
      else if (strcmp (arg, "--unshare-user-try") == 0)
        {
          opt_unshare_user_try = TRUE;
        }
      else if (strcmp (arg, "--unshare-ipc") == 0)
        {
          opt_unshare_ipc = TRUE;
        }
      else if (strcmp (arg, "--unshare-pid") == 0)
        {
          opt_unshare_pid = TRUE;
        }
      else if (strcmp (arg, "--unshare-net") == 0)
        {
          opt_unshare_net = TRUE;
        }
      else if (strcmp (arg, "--unshare-uts") == 0)
        {
          opt_unshare_uts = TRUE;
        }
      else if (strcmp (arg, "--unshare-cgroup") == 0)
        {
          opt_unshare_cgroup = TRUE;
        }
      else if (strcmp (arg, "--unshare-cgroup-try") == 0)
        {
          opt_unshare_cgroup_try = TRUE;
        }
      else if (strcmp (arg, "--chdir") == 0)
        {
          if (argc < 2)
            die ("--chdir takes one argument");

          opt_chdir_path = argv[1];
          argv++;
          argc--;
        }
      else if (strcmp (arg, "--remount-ro") == 0)
        {
          SetupOp *op = setup_op_new (SETUP_REMOUNT_RO_NO_RECURSIVE);
          op->dest = argv[1];

          argv++;
          argc--;
        }
      else if (strcmp (arg, "--bind") == 0)
        {
          if (argc < 3)
            die ("--bind takes two arguments");

          op = setup_op_new (SETUP_BIND_MOUNT);
          op->source = argv[1];
          op->dest = argv[2];

          argv += 2;
          argc -= 2;
        }
      else if (strcmp (arg, "--ro-bind") == 0)
        {
          if (argc < 3)
            die ("--ro-bind takes two arguments");

          op = setup_op_new (SETUP_RO_BIND_MOUNT);
          op->source = argv[1];
          op->dest = argv[2];

          argv += 2;
          argc -= 2;
        }
      else if (strcmp (arg, "--dev-bind") == 0)
        {
          if (argc < 3)
            die ("--dev-bind takes two arguments");

          op = setup_op_new (SETUP_DEV_BIND_MOUNT);
          op->source = argv[1];
          op->dest = argv[2];

          argv += 2;
          argc -= 2;
        }
      else if (strcmp (arg, "--proc") == 0)
        {
          if (argc < 2)
            die ("--proc takes an argument");

          op = setup_op_new (SETUP_MOUNT_PROC);
          op->dest = argv[1];

          argv += 1;
          argc -= 1;
        }
      else if (strcmp (arg, "--exec-label") == 0)
        {
          if (argc < 2)
            die ("--exec-label takes an argument");
          opt_exec_label = argv[1];
          die_unless_label_valid (opt_exec_label);

          argv += 1;
          argc -= 1;
        }
      else if (strcmp (arg, "--file-label") == 0)
        {
          if (argc < 2)
            die ("--file-label takes an argument");
          opt_file_label = argv[1];
          die_unless_label_valid (opt_file_label);
          if (label_create_file (opt_file_label))
            die_with_error ("--file-label setup failed");

          argv += 1;
          argc -= 1;
        }
      else if (strcmp (arg, "--dev") == 0)
        {
          if (argc < 2)
            die ("--dev takes an argument");

          op = setup_op_new (SETUP_MOUNT_DEV);
          op->dest = argv[1];
          opt_needs_devpts = TRUE;

          argv += 1;
          argc -= 1;
        }
      else if (strcmp (arg, "--tmpfs") == 0)
        {
          if (argc < 2)
            die ("--tmpfs takes an argument");

          op = setup_op_new (SETUP_MOUNT_TMPFS);
          op->dest = argv[1];

          argv += 1;
          argc -= 1;
        }
      else if (strcmp (arg, "--mqueue") == 0)
        {
          if (argc < 2)
            die ("--mqueue takes an argument");

          op = setup_op_new (SETUP_MOUNT_MQUEUE);
          op->dest = argv[1];

          argv += 1;
          argc -= 1;
        }
      else if (strcmp (arg, "--dir") == 0)
        {
          if (argc < 2)
            die ("--dir takes an argument");

          op = setup_op_new (SETUP_MAKE_DIR);
          op->dest = argv[1];

          argv += 1;
          argc -= 1;
        }
      else if (strcmp (arg, "--file") == 0)
        {
          int file_fd;
          char *endptr;

          if (argc < 3)
            die ("--file takes two arguments");

          file_fd = strtol (argv[1], &endptr, 10);
          if (argv[1][0] == 0 || endptr[0] != 0 || file_fd < 0)
            die ("Invalid fd: %s", argv[1]);

          op = setup_op_new (SETUP_MAKE_FILE);
          op->fd = file_fd;
          op->dest = argv[2];

          argv += 2;
          argc -= 2;
        }
      else if (strcmp (arg, "--bind-data") == 0)
        {
          int file_fd;
          char *endptr;

          if (argc < 3)
            die ("--bind-data takes two arguments");

          file_fd = strtol (argv[1], &endptr, 10);
          if (argv[1][0] == 0 || endptr[0] != 0 || file_fd < 0)
            die ("Invalid fd: %s", argv[1]);

          op = setup_op_new (SETUP_MAKE_BIND_FILE);
          op->fd = file_fd;
          op->dest = argv[2];

          argv += 2;
          argc -= 2;
        }
      else if (strcmp (arg, "--ro-bind-data") == 0)
        {
          int file_fd;
          char *endptr;

          if (argc < 3)
            die ("--ro-bind-data takes two arguments");

          file_fd = strtol (argv[1], &endptr, 10);
          if (argv[1][0] == 0 || endptr[0] != 0 || file_fd < 0)
            die ("Invalid fd: %s", argv[1]);

          op = setup_op_new (SETUP_MAKE_RO_BIND_FILE);
          op->fd = file_fd;
          op->dest = argv[2];

          argv += 2;
          argc -= 2;
        }
      else if (strcmp (arg, "--symlink") == 0)
        {
          if (argc < 3)
            die ("--symlink takes two arguments");

          op = setup_op_new (SETUP_MAKE_SYMLINK);
          op->source = argv[1];
          op->dest = argv[2];

          argv += 2;
          argc -= 2;
        }
      else if (strcmp (arg, "--lock-file") == 0)
        {
          if (argc < 2)
            die ("--lock-file takes an argument");

          (void) lock_file_new (argv[1]);

          argv += 1;
          argc -= 1;
        }
      else if (strcmp (arg, "--sync-fd") == 0)
        {
          int the_fd;
          char *endptr;

          if (argc < 2)
            die ("--sync-fd takes an argument");

          the_fd = strtol (argv[1], &endptr, 10);
          if (argv[1][0] == 0 || endptr[0] != 0 || the_fd < 0)
            die ("Invalid fd: %s", argv[1]);

          opt_sync_fd = the_fd;

          argv += 1;
          argc -= 1;
        }
      else if (strcmp (arg, "--block-fd") == 0)
        {
          int the_fd;
          char *endptr;

          if (argc < 2)
            die ("--block-fd takes an argument");

          the_fd = strtol (argv[1], &endptr, 10);
          if (argv[1][0] == 0 || endptr[0] != 0 || the_fd < 0)
            die ("Invalid fd: %s", argv[1]);

          opt_block_fd = the_fd;

          argv += 1;
          argc -= 1;
        }
      else if (strcmp (arg, "--info-fd") == 0)
        {
          int the_fd;
          char *endptr;

          if (argc < 2)
            die ("--info-fd takes an argument");

          the_fd = strtol (argv[1], &endptr, 10);
          if (argv[1][0] == 0 || endptr[0] != 0 || the_fd < 0)
            die ("Invalid fd: %s", argv[1]);

          opt_info_fd = the_fd;

          argv += 1;
          argc -= 1;
        }
      else if (strcmp (arg, "--seccomp") == 0)
        {
          int the_fd;
          char *endptr;

          if (argc < 2)
            die ("--seccomp takes an argument");

          the_fd = strtol (argv[1], &endptr, 10);
          if (argv[1][0] == 0 || endptr[0] != 0 || the_fd < 0)
            die ("Invalid fd: %s", argv[1]);

          opt_seccomp_fd = the_fd;

          argv += 1;
          argc -= 1;
        }
      else if (strcmp (arg, "--setenv") == 0)
        {
          if (argc < 3)
            die ("--setenv takes two arguments");

          xsetenv (argv[1], argv[2], 1);

          argv += 2;
          argc -= 2;
        }
      else if (strcmp (arg, "--unsetenv") == 0)
        {
          if (argc < 2)
            die ("--unsetenv takes an argument");

          xunsetenv (argv[1]);

          argv += 1;
          argc -= 1;
        }
      else if (strcmp (arg, "--uid") == 0)
        {
          int the_uid;
          char *endptr;

          if (argc < 2)
            die ("--uid takes an argument");

          the_uid = strtol (argv[1], &endptr, 10);
          if (argv[1][0] == 0 || endptr[0] != 0 || the_uid < 0)
            die ("Invalid uid: %s", argv[1]);

          opt_sandbox_uid = the_uid;

          argv += 1;
          argc -= 1;
        }
      else if (strcmp (arg, "--gid") == 0)
        {
          int the_gid;
          char *endptr;

          if (argc < 2)
            die ("--gid takes an argument");

          the_gid = strtol (argv[1], &endptr, 10);
          if (argv[1][0] == 0 || endptr[0] != 0 || the_gid < 0)
            die ("Invalid gid: %s", argv[1]);

          opt_sandbox_gid = the_gid;

          argv += 1;
          argc -= 1;
        }
      else if (strcmp (arg, "--hostname") == 0)
        {
          if (argc < 2)
            die ("--hostname takes an argument");

          op = setup_op_new (SETUP_SET_HOSTNAME);
          op->dest = argv[1];
          op->flags = NO_CREATE_DEST;

          opt_sandbox_hostname = argv[1];

          argv += 1;
          argc -= 1;
        }
      else if (*arg == '-')
        {
          die ("Unknown option %s", arg);
        }
      else
        {
          break;
        }

      argv++;
      argc--;
    }

  *argcp = argc;
  *argvp = argv;
}
