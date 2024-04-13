static int prepare_timeout(const struct timespec __user *u_abs_timeout,
			   struct timespec *ts)
{
	if (copy_from_user(ts, u_abs_timeout, sizeof(struct timespec)))
		return -EFAULT;
	if (!timespec_valid(ts))
		return -EINVAL;
	return 0;
}
