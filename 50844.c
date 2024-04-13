static int proc_id_map_open(struct inode *inode, struct file *file,
	const struct seq_operations *seq_ops)
{
	struct user_namespace *ns = NULL;
	struct task_struct *task;
	struct seq_file *seq;
	int ret = -EINVAL;

	task = get_proc_task(inode);
	if (task) {
		rcu_read_lock();
		ns = get_user_ns(task_cred_xxx(task, user_ns));
		rcu_read_unlock();
		put_task_struct(task);
	}
	if (!ns)
		goto err;

	ret = seq_open(file, seq_ops);
	if (ret)
		goto err_put_ns;

	seq = file->private_data;
	seq->private = ns;

	return 0;
err_put_ns:
	put_user_ns(ns);
err:
	return ret;
}
