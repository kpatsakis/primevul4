static ssize_t oom_score_adj_write(struct file *file, const char __user *buf,
					size_t count, loff_t *ppos)
{
	struct task_struct *task;
	char buffer[PROC_NUMBUF];
	unsigned long flags;
	int oom_score_adj;
	int err;

	memset(buffer, 0, sizeof(buffer));
	if (count > sizeof(buffer) - 1)
		count = sizeof(buffer) - 1;
	if (copy_from_user(buffer, buf, count)) {
		err = -EFAULT;
		goto out;
	}

	err = kstrtoint(strstrip(buffer), 0, &oom_score_adj);
	if (err)
		goto out;
	if (oom_score_adj < OOM_SCORE_ADJ_MIN ||
			oom_score_adj > OOM_SCORE_ADJ_MAX) {
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

	if ((short)oom_score_adj < task->signal->oom_score_adj_min &&
			!capable(CAP_SYS_RESOURCE)) {
		err = -EACCES;
		goto err_sighand;
	}

	task->signal->oom_score_adj = (short)oom_score_adj;
	if (has_capability_noaudit(current, CAP_SYS_RESOURCE))
		task->signal->oom_score_adj_min = (short)oom_score_adj;
	trace_oom_score_adj_update(task);

err_sighand:
	unlock_task_sighand(task, &flags);
err_task_lock:
	task_unlock(task);
	put_task_struct(task);
out:
	return err < 0 ? err : count;
}
