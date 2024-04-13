int create_pipe_files(struct file **res, int flags)
{
	int err;
	struct inode *inode = get_pipe_inode();
	struct file *f;
	struct path path;
	static struct qstr name = { .name = "" };

	if (!inode)
		return -ENFILE;

	err = -ENOMEM;
	path.dentry = d_alloc_pseudo(pipe_mnt->mnt_sb, &name);
	if (!path.dentry)
		goto err_inode;
	path.mnt = mntget(pipe_mnt);

	d_instantiate(path.dentry, inode);

	err = -ENFILE;
	f = alloc_file(&path, FMODE_WRITE, &pipefifo_fops);
	if (IS_ERR(f))
		goto err_dentry;

	f->f_flags = O_WRONLY | (flags & (O_NONBLOCK | O_DIRECT));
	f->private_data = inode->i_pipe;

	res[0] = alloc_file(&path, FMODE_READ, &pipefifo_fops);
	if (IS_ERR(res[0]))
		goto err_file;

	path_get(&path);
	res[0]->private_data = inode->i_pipe;
	res[0]->f_flags = O_RDONLY | (flags & O_NONBLOCK);
	res[1] = f;
	return 0;

err_file:
	put_filp(f);
err_dentry:
	free_pipe_info(inode->i_pipe);
	path_put(&path);
	return err;

err_inode:
	free_pipe_info(inode->i_pipe);
	iput(inode);
	return err;
}
