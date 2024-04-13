void free_tty_struct(struct tty_struct *tty)
{
	if (tty->dev)
		put_device(tty->dev);
	kfree(tty->write_buf);
	tty_buffer_free_all(tty);
	kfree(tty);
}
