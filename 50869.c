static int proc_task_instantiate(struct inode *dir,
	struct dentry *dentry, struct task_struct *task, const void *ptr)
{
	struct inode *inode;
	inode = proc_pid_make_inode(dir->i_sb, task);

	if (!inode)
		goto out;
	inode->i_mode = S_IFDIR|S_IRUGO|S_IXUGO;
	inode->i_op = &proc_tid_base_inode_operations;
	inode->i_fop = &proc_tid_base_operations;
	inode->i_flags|=S_IMMUTABLE;

	set_nlink(inode, 2 + pid_entry_count_dirs(tid_base_stuff,
						  ARRAY_SIZE(tid_base_stuff)));

	d_set_d_op(dentry, &pid_dentry_operations);

	d_add(dentry, inode);
	/* Close the race of the process dying before we return the dentry */
	if (pid_revalidate(dentry, 0))
		return 0;
out:
	return -ENOENT;
}
