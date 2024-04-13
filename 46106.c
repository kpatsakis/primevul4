static int ovl_copy_up_last(struct dentry *dentry, struct iattr *attr,
			    bool no_data)
{
	int err;
	struct dentry *parent;
	struct kstat stat;
	struct path lowerpath;

	parent = dget_parent(dentry);
	err = ovl_copy_up(parent);
	if (err)
		goto out_dput_parent;

	ovl_path_lower(dentry, &lowerpath);
	err = vfs_getattr(&lowerpath, &stat);
	if (err)
		goto out_dput_parent;

	if (no_data)
		stat.size = 0;

	err = ovl_copy_up_one(parent, dentry, &lowerpath, &stat, attr);

out_dput_parent:
	dput(parent);
	return err;
}
