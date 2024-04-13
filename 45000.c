static int mounts_open_common(struct inode *inode, struct file *file,
			      const struct seq_operations *op)
{
	struct task_struct *task = get_proc_task(inode);
	struct nsproxy *nsp;
	struct mnt_namespace *ns = NULL;
	struct path root;
	struct proc_mounts *p;
	int ret = -EINVAL;

	if (task) {
		rcu_read_lock();
		nsp = task_nsproxy(task);
		if (nsp) {
			ns = nsp->mnt_ns;
			if (ns)
				get_mnt_ns(ns);
		}
		rcu_read_unlock();
		if (ns && get_fs_path(task, &root, 1) == 0)
			ret = 0;
		put_task_struct(task);
	}

	if (!ns)
		goto err;
	if (ret)
		goto err_put_ns;

	ret = -ENOMEM;
	p = kmalloc(sizeof(struct proc_mounts), GFP_KERNEL);
	if (!p)
		goto err_put_path;

	file->private_data = &p->m;
	ret = seq_open(file, op);
	if (ret)
		goto err_free;

	p->m.private = p;
	p->ns = ns;
	p->root = root;
	p->event = ns->event;

	return 0;

 err_free:
	kfree(p);
 err_put_path:
	path_put(&root);
 err_put_ns:
	put_mnt_ns(ns);
 err:
	return ret;
}
