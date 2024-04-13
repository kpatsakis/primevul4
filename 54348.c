struct tty_struct *tty_pair_get_pty(struct tty_struct *tty)
{
	if (tty->driver->type == TTY_DRIVER_TYPE_PTY &&
	    tty->driver->subtype == PTY_TYPE_MASTER)
	    return tty;
	return tty->link;
}
