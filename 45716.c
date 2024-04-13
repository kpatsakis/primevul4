long __sys_recvmsg(int fd, struct msghdr __user *msg, unsigned flags)
{
	int fput_needed, err;
	struct msghdr msg_sys;
	struct socket *sock;

	sock = sockfd_lookup_light(fd, &err, &fput_needed);
	if (!sock)
		goto out;

	err = ___sys_recvmsg(sock, msg, &msg_sys, flags, 0);

	fput_light(sock->file, fput_needed);
out:
	return err;
}
