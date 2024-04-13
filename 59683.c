privileged_op (int         privileged_op_socket,
               uint32_t    op,
               uint32_t    flags,
               const char *arg1,
               const char *arg2)
{
  if (privileged_op_socket != -1)
    {
      uint32_t buffer[2048];  /* 8k, but is int32 to guarantee nice alignment */
      PrivSepOp *op_buffer = (PrivSepOp *) buffer;
      size_t buffer_size = sizeof (PrivSepOp);
      uint32_t arg1_offset = 0, arg2_offset = 0;

      /* We're unprivileged, send this request to the privileged part */

      if (arg1 != NULL)
        {
          arg1_offset = buffer_size;
          buffer_size += strlen (arg1) + 1;
        }
      if (arg2 != NULL)
        {
          arg2_offset = buffer_size;
          buffer_size += strlen (arg2) + 1;
        }

      if (buffer_size >= sizeof (buffer))
        die ("privilege separation operation to large");

      op_buffer->op = op;
      op_buffer->flags = flags;
      op_buffer->arg1_offset = arg1_offset;
      op_buffer->arg2_offset = arg2_offset;
      if (arg1 != NULL)
        strcpy ((char *) buffer + arg1_offset, arg1);
      if (arg2 != NULL)
        strcpy ((char *) buffer + arg2_offset, arg2);

      if (write (privileged_op_socket, buffer, buffer_size) != buffer_size)
        die ("Can't write to privileged_op_socket");

      if (read (privileged_op_socket, buffer, 1) != 1)
        die ("Can't read from privileged_op_socket");

      return;
    }

  /*
   * This runs a privileged request for the unprivileged setup
   * code. Note that since the setup code is unprivileged it is not as
   * trusted, so we need to verify that all requests only affect the
   * child namespace as set up by the privileged parts of the setup,
   * and that all the code is very careful about handling input.
   *
   * This means:
   *  * Bind mounts are safe, since we always use filesystem namespace. They
   *     must be recursive though, as otherwise you can use a non-recursive bind
   *     mount to access an otherwise over-mounted mountpoint.
   *  * Mounting proc, tmpfs, mqueue, devpts in the child namespace is assumed to
   *    be safe.
   *  * Remounting RO (even non-recursive) is safe because it decreases privileges.
   *  * sethostname() is safe only if we set up a UTS namespace
   */
  switch (op)
    {
    case PRIV_SEP_OP_DONE:
      break;

    case PRIV_SEP_OP_REMOUNT_RO_NO_RECURSIVE:
      if (bind_mount (proc_fd, NULL, arg2, BIND_READONLY) != 0)
        die_with_error ("Can't remount readonly on %s", arg2);
      break;

    case PRIV_SEP_OP_BIND_MOUNT:
      /* We always bind directories recursively, otherwise this would let us
         access files that are otherwise covered on the host */
      if (bind_mount (proc_fd, arg1, arg2, BIND_RECURSIVE | flags) != 0)
        die_with_error ("Can't bind mount %s on %s", arg1, arg2);
      break;

    case PRIV_SEP_OP_PROC_MOUNT:
      if (mount ("proc", arg1, "proc", MS_MGC_VAL | MS_NOSUID | MS_NOEXEC | MS_NODEV, NULL) != 0)
        die_with_error ("Can't mount proc on %s", arg1);
      break;

    case PRIV_SEP_OP_TMPFS_MOUNT:
      {
        cleanup_free char *opt = label_mount ("mode=0755", opt_file_label);
        if (mount ("tmpfs", arg1, "tmpfs", MS_MGC_VAL | MS_NOSUID | MS_NODEV, opt) != 0)
          die_with_error ("Can't mount tmpfs on %s", arg1);
        break;
      }

    case PRIV_SEP_OP_DEVPTS_MOUNT:
      if (mount ("devpts", arg1, "devpts", MS_MGC_VAL | MS_NOSUID | MS_NOEXEC,
                 "newinstance,ptmxmode=0666,mode=620") != 0)
        die_with_error ("Can't mount devpts on %s", arg1);
      break;

    case PRIV_SEP_OP_MQUEUE_MOUNT:
      if (mount ("mqueue", arg1, "mqueue", 0, NULL) != 0)
        die_with_error ("Can't mount mqueue on %s", arg1);
      break;

    case PRIV_SEP_OP_SET_HOSTNAME:
      /* This is checked at the start, but lets verify it here in case
         something manages to send hacked priv-sep operation requests. */
      if (!opt_unshare_uts)
        die ("Refusing to set hostname in original namespace");
      if (sethostname (arg1, strlen(arg1)) != 0)
        die_with_error ("Can't set hostname to %s", arg1);
      break;

    default:
      die ("Unexpected privileged op %d", op);
    }
}
