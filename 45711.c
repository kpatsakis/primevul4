SYSCALL_DEFINE5(recvmmsg, int, fd, struct mmsghdr __user *, mmsg,
		unsigned int, vlen, unsigned int, flags,
		struct timespec __user *, timeout)
{
	int datagrams;
	struct timespec timeout_sys;

	if (flags & MSG_CMSG_COMPAT)
		return -EINVAL;

	if (!timeout)
		return __sys_recvmmsg(fd, mmsg, vlen, flags, NULL);

	if (copy_from_user(&timeout_sys, timeout, sizeof(timeout_sys)))
		return -EFAULT;

	datagrams = __sys_recvmmsg(fd, mmsg, vlen, flags, &timeout_sys);

	if (datagrams > 0 &&
	    copy_to_user(timeout, &timeout_sys, sizeof(timeout_sys)))
		datagrams = -EFAULT;

	return datagrams;
}
