static int do_set_sock_timeout(struct socket *sock, int level,
		int optname, char __user *optval, unsigned int optlen)
{
	struct compat_timeval __user *up = (struct compat_timeval __user *)optval;
	struct timeval ktime;
	mm_segment_t old_fs;
	int err;

	if (optlen < sizeof(*up))
		return -EINVAL;
	if (!access_ok(VERIFY_READ, up, sizeof(*up)) ||
	    __get_user(ktime.tv_sec, &up->tv_sec) ||
	    __get_user(ktime.tv_usec, &up->tv_usec))
		return -EFAULT;
	old_fs = get_fs();
	set_fs(KERNEL_DS);
	err = sock_setsockopt(sock, level, optname, (char *)&ktime, sizeof(ktime));
	set_fs(old_fs);

	return err;
}
