struct sigqueue *sigqueue_alloc(void)
{
	struct sigqueue *q = __sigqueue_alloc(-1, current, GFP_KERNEL, 0);

	if (q)
		q->flags |= SIGQUEUE_PREALLOC;

	return q;
}
