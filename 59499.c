static inline int next_idx(int idx, struct pipe_inode_info *pipe)
{
	return (idx + 1) & (pipe->buffers - 1);
}
