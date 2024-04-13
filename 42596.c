static int apparmor_inode_getattr(struct vfsmount *mnt, struct dentry *dentry)
{
	if (!mediated_filesystem(dentry->d_inode))
		return 0;

	return common_perm_mnt_dentry(OP_GETATTR, mnt, dentry,
				      AA_MAY_META_READ);
}
