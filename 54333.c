static long tty_compat_ioctl(struct file *file, unsigned int cmd,
				unsigned long arg)
{
	struct inode *inode = file->f_dentry->d_inode;
	struct tty_struct *tty = file_tty(file);
	struct tty_ldisc *ld;
	int retval = -ENOIOCTLCMD;

	if (tty_paranoia_check(tty, inode, "tty_ioctl"))
		return -EINVAL;

	if (tty->ops->compat_ioctl) {
		retval = (tty->ops->compat_ioctl)(tty, cmd, arg);
		if (retval != -ENOIOCTLCMD)
			return retval;
	}

	ld = tty_ldisc_ref_wait(tty);
	if (ld->ops->compat_ioctl)
		retval = ld->ops->compat_ioctl(tty, file, cmd, arg);
	else
		retval = n_tty_compat_ioctl_helper(tty, file, cmd, arg);
	tty_ldisc_deref(ld);

	return retval;
}
