static struct file *do_open(struct path *path, int oflag)
{
	static const int oflag2acc[O_ACCMODE] = { MAY_READ, MAY_WRITE,
						  MAY_READ | MAY_WRITE };
	int acc;
	if ((oflag & O_ACCMODE) == (O_RDWR | O_WRONLY))
		return ERR_PTR(-EINVAL);
	acc = oflag2acc[oflag & O_ACCMODE];
	if (inode_permission(d_inode(path->dentry), acc))
		return ERR_PTR(-EACCES);
	return dentry_open(path, oflag, current_cred());
}
