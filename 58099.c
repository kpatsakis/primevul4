static ssize_t port_fops_splice_write(struct pipe_inode_info *pipe,
				      struct file *filp, loff_t *ppos,
				      size_t len, unsigned int flags)
{
	struct port *port = filp->private_data;
	struct sg_list sgl;
	ssize_t ret;
	struct port_buffer *buf;
	struct splice_desc sd = {
		.total_len = len,
		.flags = flags,
		.pos = *ppos,
		.u.data = &sgl,
	};

	/*
	 * Rproc_serial does not yet support splice. To support splice
	 * pipe_to_sg() must allocate dma-buffers and copy content from
	 * regular pages to dma pages. And alloc_buf and free_buf must
	 * support allocating and freeing such a list of dma-buffers.
	 */
	if (is_rproc_serial(port->out_vq->vdev))
		return -EINVAL;

	/*
	 * pipe->nrbufs == 0 means there are no data to transfer,
	 * so this returns just 0 for no data.
	 */
	pipe_lock(pipe);
	if (!pipe->nrbufs) {
		ret = 0;
		goto error_out;
	}

	ret = wait_port_writable(port, filp->f_flags & O_NONBLOCK);
	if (ret < 0)
		goto error_out;

	buf = alloc_buf(port->out_vq, 0, pipe->nrbufs);
	if (!buf) {
		ret = -ENOMEM;
		goto error_out;
	}

	sgl.n = 0;
	sgl.len = 0;
	sgl.size = pipe->nrbufs;
	sgl.sg = buf->sg;
	sg_init_table(sgl.sg, sgl.size);
	ret = __splice_from_pipe(pipe, &sd, pipe_to_sg);
	pipe_unlock(pipe);
	if (likely(ret > 0))
		ret = __send_to_port(port, buf->sg, sgl.n, sgl.len, buf, true);

	if (unlikely(ret <= 0))
		free_buf(buf, true);
	return ret;

error_out:
	pipe_unlock(pipe);
	return ret;
}
