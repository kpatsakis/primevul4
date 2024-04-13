static size_t push_pipe(struct iov_iter *i, size_t size,
			int *idxp, size_t *offp)
{
	struct pipe_inode_info *pipe = i->pipe;
	size_t off;
	int idx;
	ssize_t left;

	if (unlikely(size > i->count))
		size = i->count;
	if (unlikely(!size))
		return 0;

	left = size;
	data_start(i, &idx, &off);
	*idxp = idx;
	*offp = off;
	if (off) {
		left -= PAGE_SIZE - off;
		if (left <= 0) {
			pipe->bufs[idx].len += size;
			return size;
		}
		pipe->bufs[idx].len = PAGE_SIZE;
		idx = next_idx(idx, pipe);
	}
	while (idx != pipe->curbuf || !pipe->nrbufs) {
		struct page *page = alloc_page(GFP_USER);
		if (!page)
			break;
		pipe->nrbufs++;
		pipe->bufs[idx].ops = &default_pipe_buf_ops;
		pipe->bufs[idx].page = page;
		pipe->bufs[idx].offset = 0;
		if (left <= PAGE_SIZE) {
			pipe->bufs[idx].len = left;
			return size;
		}
		pipe->bufs[idx].len = PAGE_SIZE;
		left -= PAGE_SIZE;
		idx = next_idx(idx, pipe);
	}
	return size - left;
}
