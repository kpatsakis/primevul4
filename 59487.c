 void iov_iter_advance(struct iov_iter *i, size_t size)
{
	if (unlikely(i->type & ITER_PIPE)) {
		pipe_advance(i, size);
		return;
	}
	iterate_and_advance(i, size, v, 0, 0, 0)
}
