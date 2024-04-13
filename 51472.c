static int may_linkat(struct path *link)
{
	struct inode *inode;

	if (!sysctl_protected_hardlinks)
		return 0;

	inode = link->dentry->d_inode;

	/* Source inode owner (or CAP_FOWNER) can hardlink all they like,
	 * otherwise, it must be a safe source.
	 */
	if (inode_owner_or_capable(inode) || safe_hardlink_source(inode))
		return 0;

	audit_log_link_denied("linkat", link);
	return -EPERM;
}
