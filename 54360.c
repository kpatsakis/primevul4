static ssize_t tty_write(struct file *file, const char __user *buf,
						size_t count, loff_t *ppos)
{
	struct inode *inode = file->f_path.dentry->d_inode;
	struct tty_struct *tty = file_tty(file);
 	struct tty_ldisc *ld;
	ssize_t ret;

	if (tty_paranoia_check(tty, inode, "tty_write"))
		return -EIO;
	if (!tty || !tty->ops->write ||
		(test_bit(TTY_IO_ERROR, &tty->flags)))
			return -EIO;
	/* Short term debug to catch buggy drivers */
	if (tty->ops->write_room == NULL)
		printk(KERN_ERR "tty driver %s lacks a write_room method.\n",
			tty->driver->name);
	ld = tty_ldisc_ref_wait(tty);
	if (!ld->ops->write)
		ret = -EIO;
	else
		ret = do_tty_write(ld->ops->write, tty, file, buf, count);
	tty_ldisc_deref(ld);
	return ret;
}
