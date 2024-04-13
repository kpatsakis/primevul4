static long sclp_ctl_ioctl(struct file *filp, unsigned int cmd,
			   unsigned long arg)
{
	void __user *argp;

	if (is_compat_task())
		argp = compat_ptr(arg);
	else
		argp = (void __user *) arg;
	switch (cmd) {
	case SCLP_CTL_SCCB:
		return sclp_ctl_ioctl_sccb(argp);
	default: /* unknown ioctl number */
		return -ENOTTY;
	}
}
