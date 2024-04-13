void free_tty_struct(struct tty_struct *tty)
{
	if (!tty)
		return;
	put_device(tty->dev);
	kfree(tty->write_buf);
	tty->magic = 0xDEADDEAD;
	kfree(tty);
}
