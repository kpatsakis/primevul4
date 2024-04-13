static int graft_tree(struct mount *mnt, struct path *path)
{
	if (mnt->mnt.mnt_sb->s_flags & MS_NOUSER)
		return -EINVAL;

	if (S_ISDIR(path->dentry->d_inode->i_mode) !=
	      S_ISDIR(mnt->mnt.mnt_root->d_inode->i_mode))
		return -ENOTDIR;

	if (d_unlinked(path->dentry))
		return -ENOENT;

	return attach_recursive_mnt(mnt, path, NULL);
}
