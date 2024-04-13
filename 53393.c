static int tty_tiocgicount(struct tty_struct *tty, void __user *arg)
{
	int retval = -EINVAL;
	struct serial_icounter_struct icount;
	memset(&icount, 0, sizeof(icount));
	if (tty->ops->get_icount)
		retval = tty->ops->get_icount(tty, &icount);
	if (retval != 0)
		return retval;
	if (copy_to_user(arg, &icount, sizeof(icount)))
		return -EFAULT;
	return 0;
}
