static int tty_paranoia_check(struct tty_struct *tty, struct inode *inode,
			      const char *routine)
{
#ifdef TTY_PARANOIA_CHECK
	if (!tty) {
		pr_warn("(%d:%d): %s: NULL tty\n",
			imajor(inode), iminor(inode), routine);
		return 1;
	}
	if (tty->magic != TTY_MAGIC) {
		pr_warn("(%d:%d): %s: bad magic number\n",
			imajor(inode), iminor(inode), routine);
		return 1;
	}
#endif
	return 0;
}
