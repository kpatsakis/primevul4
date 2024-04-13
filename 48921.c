static inline int mctime_update_needed(const struct inode *inode,
				       const struct timespec *now)
{
	if (!timespec_equal(&inode->i_mtime, now) ||
	    !timespec_equal(&inode->i_ctime, now))
		return 1;
	return 0;
}
