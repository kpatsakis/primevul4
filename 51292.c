SYSCALL_DEFINE4(socketpair, int, family, int, type, int, protocol,
		int __user *, usockvec)
{
	struct socket *sock1, *sock2;
	int fd1, fd2, err;
	struct file *newfile1, *newfile2;
	int flags;

	flags = type & ~SOCK_TYPE_MASK;
	if (flags & ~(SOCK_CLOEXEC | SOCK_NONBLOCK))
		return -EINVAL;
	type &= SOCK_TYPE_MASK;

	if (SOCK_NONBLOCK != O_NONBLOCK && (flags & SOCK_NONBLOCK))
		flags = (flags & ~SOCK_NONBLOCK) | O_NONBLOCK;

	/*
	 * Obtain the first socket and check if the underlying protocol
	 * supports the socketpair call.
	 */

	err = sock_create(family, type, protocol, &sock1);
	if (err < 0)
		goto out;

	err = sock_create(family, type, protocol, &sock2);
	if (err < 0)
		goto out_release_1;

	err = sock1->ops->socketpair(sock1, sock2);
	if (err < 0)
		goto out_release_both;

	fd1 = get_unused_fd_flags(flags);
	if (unlikely(fd1 < 0)) {
		err = fd1;
		goto out_release_both;
	}

	fd2 = get_unused_fd_flags(flags);
	if (unlikely(fd2 < 0)) {
		err = fd2;
		goto out_put_unused_1;
	}

	newfile1 = sock_alloc_file(sock1, flags, NULL);
	if (IS_ERR(newfile1)) {
		err = PTR_ERR(newfile1);
		goto out_put_unused_both;
	}

	newfile2 = sock_alloc_file(sock2, flags, NULL);
	if (IS_ERR(newfile2)) {
		err = PTR_ERR(newfile2);
		goto out_fput_1;
	}

	err = put_user(fd1, &usockvec[0]);
	if (err)
		goto out_fput_both;

	err = put_user(fd2, &usockvec[1]);
	if (err)
		goto out_fput_both;

	audit_fd_pair(fd1, fd2);

	fd_install(fd1, newfile1);
	fd_install(fd2, newfile2);
	/* fd1 and fd2 may be already another descriptors.
	 * Not kernel problem.
	 */

	return 0;

out_fput_both:
	fput(newfile2);
	fput(newfile1);
	put_unused_fd(fd2);
	put_unused_fd(fd1);
	goto out;

out_fput_1:
	fput(newfile1);
	put_unused_fd(fd2);
	put_unused_fd(fd1);
	sock_release(sock2);
	goto out;

out_put_unused_both:
	put_unused_fd(fd2);
out_put_unused_1:
	put_unused_fd(fd1);
out_release_both:
	sock_release(sock2);
out_release_1:
	sock_release(sock1);
out:
	return err;
}
