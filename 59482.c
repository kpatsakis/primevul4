static inline void data_start(const struct iov_iter *i, int *idxp, size_t *offp)
{
	size_t off = i->iov_offset;
	int idx = i->idx;
	if (off && (!allocated(&i->pipe->bufs[idx]) || off == PAGE_SIZE)) {
		idx = next_idx(idx, i->pipe);
		off = 0;
	}
	*idxp = idx;
	*offp = off;
}
