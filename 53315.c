static int fionbio(struct file *file, int __user *p)
{
	int nonblock;

	if (get_user(nonblock, p))
		return -EFAULT;

	spin_lock(&file->f_lock);
	if (nonblock)
		file->f_flags |= O_NONBLOCK;
	else
		file->f_flags &= ~O_NONBLOCK;
	spin_unlock(&file->f_lock);
	return 0;
}
