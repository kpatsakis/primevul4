static ssize_t bm_entry_write(struct file *file, const char __user *buffer,
				size_t count, loff_t *ppos)
{
	struct dentry *root;
	Node *e = file->f_path.dentry->d_inode->i_private;
	int res = parse_command(buffer, count);

	switch (res) {
		case 1: clear_bit(Enabled, &e->flags);
			break;
		case 2: set_bit(Enabled, &e->flags);
			break;
		case 3: root = dget(file->f_path.dentry->d_sb->s_root);
			mutex_lock(&root->d_inode->i_mutex);

			kill_node(e);

			mutex_unlock(&root->d_inode->i_mutex);
			dput(root);
			break;
		default: return res;
	}
	return count;
}
