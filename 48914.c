 static int write_pipe_buf(struct pipe_inode_info *pipe, struct pipe_buffer *buf,
 			  struct splice_desc *sd)
 {
	int ret;
	void *data;
	loff_t tmp = sd->pos;

	data = kmap(buf->page);
	ret = __kernel_write(sd->u.file, data + buf->offset, sd->len, &tmp);
	kunmap(buf->page);

	return ret;
}
