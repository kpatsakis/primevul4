int vhost_add_used(struct vhost_virtqueue *vq, unsigned int head, int len)
{
	struct vring_used_elem __user *used;

	/* The virtqueue contains a ring of used buffers.  Get a pointer to the
	 * next entry in that used ring. */
	used = &vq->used->ring[vq->last_used_idx % vq->num];
	if (__put_user(head, &used->id)) {
		vq_err(vq, "Failed to write used id");
		return -EFAULT;
	}
	if (__put_user(len, &used->len)) {
		vq_err(vq, "Failed to write used len");
		return -EFAULT;
	}
	/* Make sure buffer is written before we update index. */
	smp_wmb();
	if (__put_user(vq->last_used_idx + 1, &vq->used->idx)) {
		vq_err(vq, "Failed to increment used idx");
		return -EFAULT;
	}
	if (unlikely(vq->log_used)) {
		/* Make sure data is seen before log. */
		smp_wmb();
		/* Log used ring entry write. */
		log_write(vq->log_base,
			  vq->log_addr +
			   ((void __user *)used - (void __user *)vq->used),
			  sizeof *used);
		/* Log used index update. */
		log_write(vq->log_base,
			  vq->log_addr + offsetof(struct vring_used, idx),
			  sizeof vq->used->idx);
		if (vq->log_ctx)
			eventfd_signal(vq->log_ctx, 1);
	}
	vq->last_used_idx++;
	/* If the driver never bothers to signal in a very long while,
	 * used index might wrap around. If that happens, invalidate
	 * signalled_used index we stored. TODO: make sure driver
	 * signals at least once in 2^16 and remove this. */
	if (unlikely(vq->last_used_idx == vq->signalled_used))
		vq->signalled_used_valid = false;
	return 0;
}
