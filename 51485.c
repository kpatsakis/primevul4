static inline int should_follow_link(struct nameidata *nd, struct path *link,
				     int follow,
				     struct inode *inode, unsigned seq)
{
	if (likely(!d_is_symlink(link->dentry)))
		return 0;
	if (!follow)
		return 0;
	/* make sure that d_is_symlink above matches inode */
	if (nd->flags & LOOKUP_RCU) {
		if (read_seqcount_retry(&link->dentry->d_seq, seq))
			return -ECHILD;
	}
	return pick_link(nd, link, inode, seq);
}
