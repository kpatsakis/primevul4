void pipe_lock(struct pipe_inode_info *pipe)
{
	/*
	 * pipe_lock() nests non-pipe inode locks (for writing to a file)
	 */
	pipe_lock_nested(pipe, I_MUTEX_PARENT);
}
