static inline void __pipe_unlock(struct pipe_inode_info *pipe)
{
	mutex_unlock(&pipe->mutex);
}
