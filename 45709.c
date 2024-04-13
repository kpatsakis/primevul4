SYSCALL_DEFINE3(recvmsg, int, fd, struct msghdr __user *, msg,
		unsigned int, flags)
{
	if (flags & MSG_CMSG_COMPAT)
		return -EINVAL;
	return __sys_recvmsg(fd, msg, flags);
}
