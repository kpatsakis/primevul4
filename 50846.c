static inline bool proc_inode_is_dead(struct inode *inode)
{
	return !proc_pid(inode)->tasks[PIDTYPE_PID].first;
}
