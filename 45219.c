static int do_get_sock_timeout(struct socket *sock, int level, int optname,
		char __user *optval, int __user *optlen)
{
	struct compat_timeval __user *up;
	struct timeval ktime;
	mm_segment_t old_fs;
	int len, err;

	up = (struct compat_timeval __user *) optval;
	if (get_user(len, optlen))
		return -EFAULT;
	if (len < sizeof(*up))
		return -EINVAL;
	len = sizeof(ktime);
	old_fs = get_fs();
	set_fs(KERNEL_DS);
	err = sock_getsockopt(sock, level, optname, (char *) &ktime, &len);
	set_fs(old_fs);

	if (!err) {
		if (put_user(sizeof(*up), optlen) ||
		    !access_ok(VERIFY_WRITE, up, sizeof(*up)) ||
		    __put_user(ktime.tv_sec, &up->tv_sec) ||
		    __put_user(ktime.tv_usec, &up->tv_usec))
			err = -EFAULT;
	}
	return err;
}
