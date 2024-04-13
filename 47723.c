void generic_pipe_buf_get(struct pipe_inode_info *pipe, struct pipe_buffer *buf)
{
	page_cache_get(buf->page);
}
