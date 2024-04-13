static void pipe_lock_nested(struct pipe_inode_info *pipe, int subclass)
{
	if (pipe->files)
		mutex_lock_nested(&pipe->mutex, subclass);
}
