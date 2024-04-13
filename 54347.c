char *tty_name(struct tty_struct *tty, char *buf)
{
	if (!tty) /* Hmm.  NULL pointer.  That's fun. */
		strcpy(buf, "NULL tty");
	else
		strcpy(buf, tty->name);
	return buf;
}
