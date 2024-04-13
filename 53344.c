static int tiocswinsz(struct tty_struct *tty, struct winsize __user *arg)
{
	struct winsize tmp_ws;
	if (copy_from_user(&tmp_ws, arg, sizeof(*arg)))
		return -EFAULT;

	if (tty->ops->resize)
		return tty->ops->resize(tty, &tmp_ws);
	else
		return tty_do_resize(tty, &tmp_ws);
}
