const char *tty_name(const struct tty_struct *tty)
{
	if (!tty) /* Hmm.  NULL pointer.  That's fun. */
		return "NULL tty";
	return tty->name;
}
