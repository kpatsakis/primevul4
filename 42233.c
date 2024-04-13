void vhost_work_init(struct vhost_work *work, vhost_work_fn_t fn)
{
	INIT_LIST_HEAD(&work->node);
	work->fn = fn;
	init_waitqueue_head(&work->done);
	work->flushing = 0;
	work->queue_seq = work->done_seq = 0;
}
