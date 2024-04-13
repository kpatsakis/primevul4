static int proc_root_link(struct inode *inode, struct path *path)
{
	struct task_struct *task = get_proc_task(inode);
	int result = -ENOENT;

	if (task) {
		result = get_fs_path(task, path, 1);
		put_task_struct(task);
	}
	return result;
}
