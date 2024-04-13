struct file *shmem_file_setup(const char *name, loff_t size, unsigned long flags)
{
	int error;
	struct file *file;
	struct inode *inode;
	struct path path;
	struct dentry *root;
	struct qstr this;

	if (IS_ERR(shm_mnt))
		return (void *)shm_mnt;

	if (size < 0 || size > MAX_LFS_FILESIZE)
		return ERR_PTR(-EINVAL);

	if (shmem_acct_size(flags, size))
		return ERR_PTR(-ENOMEM);

	error = -ENOMEM;
	this.name = name;
	this.len = strlen(name);
	this.hash = 0; /* will go */
	root = shm_mnt->mnt_root;
	path.dentry = d_alloc(root, &this);
	if (!path.dentry)
		goto put_memory;
	path.mnt = mntget(shm_mnt);

	error = -ENOSPC;
	inode = shmem_get_inode(root->d_sb, NULL, S_IFREG | S_IRWXUGO, 0, flags);
	if (!inode)
		goto put_dentry;

	d_instantiate(path.dentry, inode);
	inode->i_size = size;
	clear_nlink(inode);	/* It is unlinked */
#ifndef CONFIG_MMU
	error = ramfs_nommu_expand_for_mapping(inode, size);
	if (error)
		goto put_dentry;
#endif

	error = -ENFILE;
	file = alloc_file(&path, FMODE_WRITE | FMODE_READ,
		  &shmem_file_operations);
	if (!file)
		goto put_dentry;

	return file;

put_dentry:
	path_put(&path);
put_memory:
	shmem_unacct_size(flags, size);
	return ERR_PTR(error);
}
