ssize_t redirected_tty_write(struct file *file, const char __user *buf,
						size_t count, loff_t *ppos)
{
	struct file *p = NULL;

	spin_lock(&redirect_lock);
	if (redirect) {
		get_file(redirect);
		p = redirect;
	}
	spin_unlock(&redirect_lock);

	if (p) {
		ssize_t res;
		res = vfs_write(p, buf, count, &p->f_pos);
		fput(p);
		return res;
	}
	return tty_write(file, buf, count, ppos);
}
