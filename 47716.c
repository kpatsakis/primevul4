static inline void __pipe_lock(struct pipe_inode_info *pipe)
{
	mutex_lock_nested(&pipe->mutex, I_MUTEX_PARENT);
}
