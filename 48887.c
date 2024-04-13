static int generic_pipe_buf_nosteal(struct pipe_inode_info *pipe,
				    struct pipe_buffer *buf)
{
	return 1;
}
