void pipe_unlock(struct pipe_inode_info *pipe)
{
	if (pipe->files)
		mutex_unlock(&pipe->mutex);
}
