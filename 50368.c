static struct dentry *proc_sys_lookup(struct inode *dir, struct dentry *dentry,
					unsigned int flags)
{
	struct ctl_table_header *head = grab_header(dir);
	struct ctl_table_header *h = NULL;
	const struct qstr *name = &dentry->d_name;
	struct ctl_table *p;
	struct inode *inode;
	struct dentry *err = ERR_PTR(-ENOENT);
	struct ctl_dir *ctl_dir;
	int ret;

	if (IS_ERR(head))
		return ERR_CAST(head);

	ctl_dir = container_of(head, struct ctl_dir, header);

	p = lookup_entry(&h, ctl_dir, name->name, name->len);
	if (!p)
		goto out;

	if (S_ISLNK(p->mode)) {
		ret = sysctl_follow_link(&h, &p);
		err = ERR_PTR(ret);
		if (ret)
			goto out;
	}

	err = ERR_PTR(-ENOMEM);
	inode = proc_sys_make_inode(dir->i_sb, h ? h : head, p);
	if (!inode)
		goto out;

	err = NULL;
	d_set_d_op(dentry, &proc_sys_dentry_operations);
	d_add(dentry, inode);

out:
	if (h)
		sysctl_head_finish(h);
	sysctl_head_finish(head);
	return err;
}
