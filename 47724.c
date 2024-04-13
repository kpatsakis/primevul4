void generic_pipe_buf_release(struct pipe_inode_info *pipe,
			      struct pipe_buffer *buf)
{
	page_cache_release(buf->page);
}
