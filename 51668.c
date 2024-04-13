static void airspy_buf_queue(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct airspy *s = vb2_get_drv_priv(vb->vb2_queue);
	struct airspy_frame_buf *buf =
			container_of(vbuf, struct airspy_frame_buf, vb);
	unsigned long flags;

	/* Check the device has not disconnected between prep and queuing */
	if (unlikely(!s->udev)) {
		vb2_buffer_done(&buf->vb.vb2_buf, VB2_BUF_STATE_ERROR);
		return;
	}

	spin_lock_irqsave(&s->queued_bufs_lock, flags);
	list_add_tail(&buf->list, &s->queued_bufs);
	spin_unlock_irqrestore(&s->queued_bufs_lock, flags);
}
