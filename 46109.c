static int ovl_getattr(struct vfsmount *mnt, struct dentry *dentry,
			 struct kstat *stat)
{
	struct path realpath;

	ovl_path_real(dentry, &realpath);
	return vfs_getattr(&realpath, stat);
}
