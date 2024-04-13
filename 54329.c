static int tiocgwinsz(struct tty_struct *tty, struct winsize __user *arg)
{
	int err;

	mutex_lock(&tty->termios_mutex);
	err = copy_to_user(arg, &tty->winsize, sizeof(*arg));
	mutex_unlock(&tty->termios_mutex);

	return err ? -EFAULT: 0;
}
