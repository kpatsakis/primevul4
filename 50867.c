static int proc_setgroups_release(struct inode *inode, struct file *file)
{
	struct seq_file *seq = file->private_data;
	struct user_namespace *ns = seq->private;
	int ret = single_release(inode, file);
	put_user_ns(ns);
	return ret;
}
