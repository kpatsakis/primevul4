long __sys_sendmsg(int fd, struct msghdr __user *msg, unsigned flags)
{
	int fput_needed, err;
	struct msghdr msg_sys;
	struct socket *sock;

	sock = sockfd_lookup_light(fd, &err, &fput_needed);
	if (!sock)
		goto out;

	err = ___sys_sendmsg(sock, msg, &msg_sys, flags, NULL);

	fput_light(sock->file, fput_needed);
out:
	return err;
}
