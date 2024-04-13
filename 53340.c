static int tiocgwinsz(struct tty_struct *tty, struct winsize __user *arg)
{
	int err;

	mutex_lock(&tty->winsize_mutex);
	err = copy_to_user(arg, &tty->winsize, sizeof(*arg));
	mutex_unlock(&tty->winsize_mutex);

	return err ? -EFAULT: 0;
}
