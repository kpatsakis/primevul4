static int proc_task_getattr(struct vfsmount *mnt, struct dentry *dentry, struct kstat *stat)
{
	struct inode *inode = d_inode(dentry);
	struct task_struct *p = get_proc_task(inode);
	generic_fillattr(inode, stat);

	if (p) {
		stat->nlink += get_nr_threads(p);
		put_task_struct(p);
	}

	return 0;
}
