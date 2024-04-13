static int sctp_getsockopt_peeloff_common(struct sock *sk, sctp_peeloff_arg_t *peeloff,
					  struct file **newfile, unsigned flags)
{
	struct socket *newsock;
	int retval;

	retval = sctp_do_peeloff(sk, peeloff->associd, &newsock);
	if (retval < 0)
		goto out;

	/* Map the socket to an unused fd that can be returned to the user.  */
	retval = get_unused_fd_flags(flags & SOCK_CLOEXEC);
	if (retval < 0) {
		sock_release(newsock);
		goto out;
	}

	*newfile = sock_alloc_file(newsock, 0, NULL);
	if (IS_ERR(*newfile)) {
		put_unused_fd(retval);
		sock_release(newsock);
		retval = PTR_ERR(*newfile);
		*newfile = NULL;
		return retval;
	}

	pr_debug("%s: sk:%p, newsk:%p, sd:%d\n", __func__, sk, newsock->sk,
		 retval);

	peeloff->sd = retval;

	if (flags & SOCK_NONBLOCK)
		(*newfile)->f_flags |= O_NONBLOCK;
out:
	return retval;
}
