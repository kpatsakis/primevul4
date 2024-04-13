static int tty_tiocmget(struct tty_struct *tty, int __user *p)
{
	int retval = -EINVAL;

	if (tty->ops->tiocmget) {
		retval = tty->ops->tiocmget(tty);

		if (retval >= 0)
			retval = put_user(retval, p);
	}
	return retval;
}
