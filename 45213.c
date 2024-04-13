asmlinkage long compat_sys_recv(int fd, void __user *buf, size_t len, unsigned int flags)
{
	return sys_recv(fd, buf, len, flags | MSG_CMSG_COMPAT);
}
