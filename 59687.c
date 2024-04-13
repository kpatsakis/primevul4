setup_newroot (bool unshare_pid,
               int  privileged_op_socket)
{
  SetupOp *op;

  for (op = ops; op != NULL; op = op->next)
    {
      cleanup_free char *source = NULL;
      cleanup_free char *dest = NULL;
      int source_mode = 0;
      int i;

      if (op->source &&
          op->type != SETUP_MAKE_SYMLINK)
        {
          source = get_oldroot_path (op->source);
          source_mode = get_file_mode (source);
          if (source_mode < 0)
            die_with_error ("Can't get type of source %s", op->source);
        }

      if (op->dest &&
          (op->flags & NO_CREATE_DEST) == 0)
        {
          dest = get_newroot_path (op->dest);
          if (mkdir_with_parents (dest, 0755, FALSE) != 0)
            die_with_error ("Can't mkdir parents for %s", op->dest);
        }

      switch (op->type)
        {
        case SETUP_RO_BIND_MOUNT:
        case SETUP_DEV_BIND_MOUNT:
        case SETUP_BIND_MOUNT:
          if (source_mode == S_IFDIR)
            {
              if (mkdir (dest, 0755) != 0 && errno != EEXIST)
                die_with_error ("Can't mkdir %s", op->dest);
            }
          else if (ensure_file (dest, 0666) != 0)
            die_with_error ("Can't create file at %s", op->dest);

          privileged_op (privileged_op_socket,
                         PRIV_SEP_OP_BIND_MOUNT,
                         (op->type == SETUP_RO_BIND_MOUNT ? BIND_READONLY : 0) |
                         (op->type == SETUP_DEV_BIND_MOUNT ? BIND_DEVICES : 0),
                         source, dest);
          break;

        case SETUP_REMOUNT_RO_NO_RECURSIVE:
          privileged_op (privileged_op_socket,
                         PRIV_SEP_OP_REMOUNT_RO_NO_RECURSIVE, BIND_READONLY, NULL, dest);
          break;

        case SETUP_MOUNT_PROC:
          if (mkdir (dest, 0755) != 0 && errno != EEXIST)
            die_with_error ("Can't mkdir %s", op->dest);

          if (unshare_pid)
            {
              /* Our own procfs */
              privileged_op (privileged_op_socket,
                             PRIV_SEP_OP_PROC_MOUNT, 0,
                             dest, NULL);
            }
          else
            {
              /* Use system procfs, as we share pid namespace anyway */
              privileged_op (privileged_op_socket,
                             PRIV_SEP_OP_BIND_MOUNT, 0,
                             "oldroot/proc", dest);
            }

          /* There are a bunch of weird old subdirs of /proc that could potentially be
             problematic (for instance /proc/sysrq-trigger lets you shut down the machine
             if you have write access). We should not have access to these as a non-privileged
             user, but lets cover them anyway just to make sure */
          const char *cover_proc_dirs[] = { "sys", "sysrq-trigger", "irq", "bus" };
          for (i = 0; i < N_ELEMENTS (cover_proc_dirs); i++)
            {
              cleanup_free char *subdir = strconcat3 (dest, "/", cover_proc_dirs[i]);
              privileged_op (privileged_op_socket,
                             PRIV_SEP_OP_BIND_MOUNT, BIND_READONLY,
                             subdir, subdir);
            }

          break;

        case SETUP_MOUNT_DEV:
          if (mkdir (dest, 0755) != 0 && errno != EEXIST)
            die_with_error ("Can't mkdir %s", op->dest);

          privileged_op (privileged_op_socket,
                         PRIV_SEP_OP_TMPFS_MOUNT, 0,
                         dest, NULL);

          static const char *const devnodes[] = { "null", "zero", "full", "random", "urandom", "tty" };
          for (i = 0; i < N_ELEMENTS (devnodes); i++)
            {
              cleanup_free char *node_dest = strconcat3 (dest, "/", devnodes[i]);
              cleanup_free char *node_src = strconcat ("/oldroot/dev/", devnodes[i]);
              if (create_file (node_dest, 0666, NULL) != 0)
                die_with_error ("Can't create file %s/%s", op->dest, devnodes[i]);
              privileged_op (privileged_op_socket,
                             PRIV_SEP_OP_BIND_MOUNT, BIND_DEVICES,
                             node_src, node_dest);
            }

          static const char *const stdionodes[] = { "stdin", "stdout", "stderr" };
          for (i = 0; i < N_ELEMENTS (stdionodes); i++)
            {
              cleanup_free char *target = xasprintf ("/proc/self/fd/%d", i);
              cleanup_free char *node_dest = strconcat3 (dest, "/", stdionodes[i]);
              if (symlink (target, node_dest) < 0)
                die_with_error ("Can't create symlink %s/%s", op->dest, stdionodes[i]);
            }

          {
            cleanup_free char *pts = strconcat (dest, "/pts");
            cleanup_free char *ptmx = strconcat (dest, "/ptmx");
            cleanup_free char *shm = strconcat (dest, "/shm");

            if (mkdir (shm, 0755) == -1)
              die_with_error ("Can't create %s/shm", op->dest);

            if (mkdir (pts, 0755) == -1)
              die_with_error ("Can't create %s/devpts", op->dest);
            privileged_op (privileged_op_socket,
                           PRIV_SEP_OP_DEVPTS_MOUNT, BIND_DEVICES,
                           pts, NULL);

            if (symlink ("pts/ptmx", ptmx) != 0)
              die_with_error ("Can't make symlink at %s/ptmx", op->dest);
          }

          /* If stdout is a tty, that means the sandbox can write to the
             outside-sandbox tty. In that case we also create a /dev/console
             that points to this tty device. This should not cause any more
             access than we already have, and it makes ttyname() work in the
             sandbox. */
          if (host_tty_dev != NULL && *host_tty_dev != 0)
            {
              cleanup_free char *src_tty_dev = strconcat ("/oldroot", host_tty_dev);
              cleanup_free char *dest_console = strconcat (dest, "/console");

              if (create_file (dest_console, 0666, NULL) != 0)
                die_with_error ("creating %s/console", op->dest);

              privileged_op (privileged_op_socket,
                             PRIV_SEP_OP_BIND_MOUNT, BIND_DEVICES,
                             src_tty_dev, dest_console);
            }

          break;

        case SETUP_MOUNT_TMPFS:
          if (mkdir (dest, 0755) != 0 && errno != EEXIST)
            die_with_error ("Can't mkdir %s", op->dest);

          privileged_op (privileged_op_socket,
                         PRIV_SEP_OP_TMPFS_MOUNT, 0,
                         dest, NULL);
          break;

        case SETUP_MOUNT_MQUEUE:
          if (mkdir (dest, 0755) != 0 && errno != EEXIST)
            die_with_error ("Can't mkdir %s", op->dest);

          privileged_op (privileged_op_socket,
                         PRIV_SEP_OP_MQUEUE_MOUNT, 0,
                         dest, NULL);
          break;

        case SETUP_MAKE_DIR:
          if (mkdir (dest, 0755) != 0 && errno != EEXIST)
            die_with_error ("Can't mkdir %s", op->dest);

          break;

        case SETUP_MAKE_FILE:
          {
            cleanup_fd int dest_fd = -1;

            dest_fd = creat (dest, 0666);
            if (dest_fd == -1)
              die_with_error ("Can't create file %s", op->dest);

            if (copy_file_data (op->fd, dest_fd) != 0)
              die_with_error ("Can't write data to file %s", op->dest);

            close (op->fd);
          }
          break;

        case SETUP_MAKE_BIND_FILE:
        case SETUP_MAKE_RO_BIND_FILE:
          {
            cleanup_fd int dest_fd = -1;
            char tempfile[] = "/bindfileXXXXXX";

            dest_fd = mkstemp (tempfile);
            if (dest_fd == -1)
              die_with_error ("Can't create tmpfile for %s", op->dest);

            if (copy_file_data (op->fd, dest_fd) != 0)
              die_with_error ("Can't write data to file %s", op->dest);

            close (op->fd);

            if (ensure_file (dest, 0666) != 0)
              die_with_error ("Can't create file at %s", op->dest);

            privileged_op (privileged_op_socket,
                           PRIV_SEP_OP_BIND_MOUNT,
                           (op->type == SETUP_MAKE_RO_BIND_FILE ? BIND_READONLY : 0),
                           tempfile, dest);

            /* Remove the file so we're sure the app can't get to it in any other way.
               Its outside the container chroot, so it shouldn't be possible, but lets
               make it really sure. */
            unlink (tempfile);
          }
          break;

        case SETUP_MAKE_SYMLINK:
          if (symlink (op->source, dest) != 0)
            die_with_error ("Can't make symlink at %s", op->dest);
          break;

        case SETUP_SET_HOSTNAME:
          privileged_op (privileged_op_socket,
                         PRIV_SEP_OP_SET_HOSTNAME, 0,
                         op->dest, NULL);
          break;

        default:
          die ("Unexpected type %d", op->type);
        }
    }
  privileged_op (privileged_op_socket,
                 PRIV_SEP_OP_DONE, 0, NULL, NULL);
}
