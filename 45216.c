asmlinkage long compat_sys_sendmsg(int fd, struct compat_msghdr __user *msg, unsigned int flags)
{
	if (flags & MSG_CMSG_COMPAT)
		return -EINVAL;
	return __sys_sendmsg(fd, (struct msghdr __user *)msg, flags | MSG_CMSG_COMPAT);
}
