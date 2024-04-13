static void airspy_cleanup_queued_bufs(struct airspy *s)
{
	unsigned long flags;

	dev_dbg(s->dev, "\n");

	spin_lock_irqsave(&s->queued_bufs_lock, flags);
	while (!list_empty(&s->queued_bufs)) {
		struct airspy_frame_buf *buf;

		buf = list_entry(s->queued_bufs.next,
				struct airspy_frame_buf, list);
		list_del(&buf->list);
		vb2_buffer_done(&buf->vb.vb2_buf, VB2_BUF_STATE_ERROR);
	}
	spin_unlock_irqrestore(&s->queued_bufs_lock, flags);
}
