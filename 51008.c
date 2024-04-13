static struct dentry *ffs_sb_create_file(struct super_block *sb,
					const char *name, void *data,
					const struct file_operations *fops)
{
	struct ffs_data	*ffs = sb->s_fs_info;
	struct dentry	*dentry;
	struct inode	*inode;

	ENTER();

	dentry = d_alloc_name(sb->s_root, name);
	if (unlikely(!dentry))
		return NULL;

	inode = ffs_sb_make_inode(sb, data, fops, NULL, &ffs->file_perms);
	if (unlikely(!inode)) {
		dput(dentry);
		return NULL;
	}

	d_add(dentry, inode);
	return dentry;
}
