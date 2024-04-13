static inline int should_follow_link(struct dentry *dentry, int follow)
{
	return unlikely(d_is_symlink(dentry)) ? follow : 0;
}
