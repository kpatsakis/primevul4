static ssize_t proc_pid_attr_write(struct file * file, const char __user * buf,
				   size_t count, loff_t *ppos)
{
	struct inode * inode = file_inode(file);
	void *page;
	ssize_t length;
	struct task_struct *task = get_proc_task(inode);

	length = -ESRCH;
	if (!task)
		goto out_no_task;
	if (count > PAGE_SIZE)
		count = PAGE_SIZE;

	/* No partial writes. */
	length = -EINVAL;
	if (*ppos != 0)
		goto out;

	page = memdup_user(buf, count);
	if (IS_ERR(page)) {
		length = PTR_ERR(page);
		goto out;
	}

	/* Guard against adverse ptrace interaction */
	length = mutex_lock_interruptible(&task->signal->cred_guard_mutex);
	if (length < 0)
		goto out_free;

	length = security_setprocattr(task,
				      (char*)file->f_path.dentry->d_name.name,
				      page, count);
	mutex_unlock(&task->signal->cred_guard_mutex);
out_free:
	kfree(page);
out:
	put_task_struct(task);
out_no_task:
	return length;
}
