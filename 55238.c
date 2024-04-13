static int sctp_getsockopt_peeloff_flags(struct sock *sk, int len,
					 char __user *optval, int __user *optlen)
{
	sctp_peeloff_flags_arg_t peeloff;
	struct file *newfile = NULL;
	int retval = 0;

	if (len < sizeof(sctp_peeloff_flags_arg_t))
		return -EINVAL;
	len = sizeof(sctp_peeloff_flags_arg_t);
	if (copy_from_user(&peeloff, optval, len))
		return -EFAULT;

	retval = sctp_getsockopt_peeloff_common(sk, &peeloff.p_arg,
						&newfile, peeloff.flags);
	if (retval < 0)
		goto out;

	/* Return the fd mapped to the new socket.  */
	if (put_user(len, optlen)) {
		fput(newfile);
		put_unused_fd(retval);
		return -EFAULT;
	}

	if (copy_to_user(optval, &peeloff, len)) {
		fput(newfile);
		put_unused_fd(retval);
		return -EFAULT;
	}
	fd_install(retval, newfile);
out:
	return retval;
}
