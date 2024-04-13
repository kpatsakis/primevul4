static int sctp_getsockopt_peeloff(struct sock *sk, int len, char __user *optval, int __user *optlen)
{
	sctp_peeloff_arg_t peeloff;
	struct socket *newsock;
	struct file *newfile;
	int retval = 0;

	if (len < sizeof(sctp_peeloff_arg_t))
		return -EINVAL;
	len = sizeof(sctp_peeloff_arg_t);
	if (copy_from_user(&peeloff, optval, len))
		return -EFAULT;

	retval = sctp_do_peeloff(sk, peeloff.associd, &newsock);
	if (retval < 0)
		goto out;

	/* Map the socket to an unused fd that can be returned to the user.  */
	retval = get_unused_fd();
	if (retval < 0) {
		sock_release(newsock);
		goto out;
	}

	newfile = sock_alloc_file(newsock, 0, NULL);
	if (unlikely(IS_ERR(newfile))) {
		put_unused_fd(retval);
		sock_release(newsock);
		return PTR_ERR(newfile);
	}

	SCTP_DEBUG_PRINTK("%s: sk: %p newsk: %p sd: %d\n",
			  __func__, sk, newsock->sk, retval);

	/* Return the fd mapped to the new socket.  */
	if (put_user(len, optlen)) {
		fput(newfile);
		put_unused_fd(retval);
		return -EFAULT;
	}
	peeloff.sd = retval;
	if (copy_to_user(optval, &peeloff, len)) {
		fput(newfile);
		put_unused_fd(retval);
		return -EFAULT;
	}
	fd_install(retval, newfile);
out:
	return retval;
}
