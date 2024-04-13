int vc4_queue_seqno_cb(struct drm_device *dev,
		       struct vc4_seqno_cb *cb, uint64_t seqno,
		       void (*func)(struct vc4_seqno_cb *cb))
{
	struct vc4_dev *vc4 = to_vc4_dev(dev);
	int ret = 0;
	unsigned long irqflags;

	cb->func = func;
	INIT_WORK(&cb->work, vc4_seqno_cb_work);

	spin_lock_irqsave(&vc4->job_lock, irqflags);
	if (seqno > vc4->finished_seqno) {
		cb->seqno = seqno;
		list_add_tail(&cb->work.entry, &vc4->seqno_cb_list);
	} else {
		schedule_work(&cb->work);
	}
	spin_unlock_irqrestore(&vc4->job_lock, irqflags);

	return ret;
}
