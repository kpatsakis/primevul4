static ssize_t bm_register_write(struct file *file, const char __user *buffer,
			       size_t count, loff_t *ppos)
{
	Node *e;
	struct inode *inode;
	struct dentry *root, *dentry;
	struct super_block *sb = file->f_path.dentry->d_sb;
	int err = 0;

	e = create_entry(buffer, count);

	if (IS_ERR(e))
		return PTR_ERR(e);

	root = dget(sb->s_root);
	mutex_lock(&root->d_inode->i_mutex);
	dentry = lookup_one_len(e->name, root, strlen(e->name));
	err = PTR_ERR(dentry);
	if (IS_ERR(dentry))
		goto out;

	err = -EEXIST;
	if (dentry->d_inode)
		goto out2;

	inode = bm_get_inode(sb, S_IFREG | 0644);

	err = -ENOMEM;
	if (!inode)
		goto out2;

	err = simple_pin_fs(&bm_fs_type, &bm_mnt, &entry_count);
	if (err) {
		iput(inode);
		inode = NULL;
		goto out2;
	}

	e->dentry = dget(dentry);
	inode->i_private = e;
	inode->i_fop = &bm_entry_operations;

	d_instantiate(dentry, inode);
	write_lock(&entries_lock);
	list_add(&e->list, &entries);
	write_unlock(&entries_lock);

	err = 0;
out2:
	dput(dentry);
out:
	mutex_unlock(&root->d_inode->i_mutex);
	dput(root);

	if (err) {
		kfree(e);
		return -EINVAL;
	}
	return count;
}
