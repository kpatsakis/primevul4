asmlinkage long compat_sys_recvfrom(int fd, void __user *buf, size_t len,
				    unsigned int flags, struct sockaddr __user *addr,
				    int __user *addrlen)
{
	return sys_recvfrom(fd, buf, len, flags | MSG_CMSG_COMPAT, addr, addrlen);
}
