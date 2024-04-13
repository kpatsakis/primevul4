static bool sanity(const struct iov_iter *i)
{
	struct pipe_inode_info *pipe = i->pipe;
	int idx = i->idx;
	int next = pipe->curbuf + pipe->nrbufs;
	if (i->iov_offset) {
		struct pipe_buffer *p;
		if (unlikely(!pipe->nrbufs))
			goto Bad;	// pipe must be non-empty
		if (unlikely(idx != ((next - 1) & (pipe->buffers - 1))))
			goto Bad;	// must be at the last buffer...

		p = &pipe->bufs[idx];
		if (unlikely(p->offset + p->len != i->iov_offset))
			goto Bad;	// ... at the end of segment
	} else {
		if (idx != (next & (pipe->buffers - 1)))
			goto Bad;	// must be right after the last buffer
	}
	return true;
Bad:
	printk(KERN_ERR "idx = %d, offset = %zd\n", i->idx, i->iov_offset);
	printk(KERN_ERR "curbuf = %d, nrbufs = %d, buffers = %d\n",
			pipe->curbuf, pipe->nrbufs, pipe->buffers);
	for (idx = 0; idx < pipe->buffers; idx++)
		printk(KERN_ERR "[%p %p %d %d]\n",
			pipe->bufs[idx].ops,
			pipe->bufs[idx].page,
			pipe->bufs[idx].offset,
			pipe->bufs[idx].len);
	WARN_ON(1);
	return false;
}
