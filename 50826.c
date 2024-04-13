static ssize_t oom_adj_write(struct file *file, const char __user *buf,
			     size_t count, loff_t *ppos)
{
	struct task_struct *task;
	char buffer[PROC_NUMBUF];
	int oom_adj;
	unsigned long flags;
	int err;

	memset(buffer, 0, sizeof(buffer));
	if (count > sizeof(buffer) - 1)
		count = sizeof(buffer) - 1;
	if (copy_from_user(buffer, buf, count)) {
		err = -EFAULT;
		goto out;
	}

	err = kstrtoint(strstrip(buffer), 0, &oom_adj);
	if (err)
		goto out;
	if ((oom_adj < OOM_ADJUST_MIN || oom_adj > OOM_ADJUST_MAX) &&
	     oom_adj != OOM_DISABLE) {
		err = -EINVAL;
		goto out;
	}

	task = get_proc_task(file_inode(file));
	if (!task) {
		err = -ESRCH;
		goto out;
	}

	task_lock(task);
	if (!task->mm) {
		err = -EINVAL;
		goto err_task_lock;
	}

	if (!lock_task_sighand(task, &flags)) {
		err = -ESRCH;
		goto err_task_lock;
	}

	/*
	 * Scale /proc/pid/oom_score_adj appropriately ensuring that a maximum
	 * value is always attainable.
	 */
	if (oom_adj == OOM_ADJUST_MAX)
		oom_adj = OOM_SCORE_ADJ_MAX;
	else
		oom_adj = (oom_adj * OOM_SCORE_ADJ_MAX) / -OOM_DISABLE;

	if (oom_adj < task->signal->oom_score_adj &&
	    !capable(CAP_SYS_RESOURCE)) {
		err = -EACCES;
		goto err_sighand;
	}

	/*
	 * /proc/pid/oom_adj is provided for legacy purposes, ask users to use
	 * /proc/pid/oom_score_adj instead.
	 */
	pr_warn_once("%s (%d): /proc/%d/oom_adj is deprecated, please use /proc/%d/oom_score_adj instead.\n",
		  current->comm, task_pid_nr(current), task_pid_nr(task),
		  task_pid_nr(task));

	task->signal->oom_score_adj = oom_adj;
	trace_oom_score_adj_update(task);
err_sighand:
	unlock_task_sighand(task, &flags);
err_task_lock:
	task_unlock(task);
	put_task_struct(task);
out:
	return err < 0 ? err : count;
}
