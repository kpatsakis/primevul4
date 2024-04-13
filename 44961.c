static struct dentry *lookup_hash(struct nameidata *nd)
{
	int err;

	err = exec_permission(nd->path.dentry->d_inode);
	if (err)
		return ERR_PTR(err);
	return __lookup_hash(&nd->last, nd->path.dentry, nd);
}
