static int compat_prepare_timeout(const struct compat_timespec __user *p,
				   struct timespec *ts)
{
	if (compat_get_timespec(ts, p))
		return -EFAULT;
	if (!timespec_valid(ts))
		return -EINVAL;
	return 0;
}
