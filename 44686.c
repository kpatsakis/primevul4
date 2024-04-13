static inline int fd_copyin(void __user *param, void *address,
			    unsigned long size)
{
	return copy_from_user(address, param, size) ? -EFAULT : 0;
}
