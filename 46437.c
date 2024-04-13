static int __vhost_add_used_n(struct vhost_virtqueue *vq,
			    struct vring_used_elem *heads,
			    unsigned count)
{
	struct vring_used_elem __user *used;
	u16 old, new;
	int start;

	start = vq->last_used_idx % vq->num;
	used = vq->used->ring + start;
	if (count == 1) {
		if (__put_user(heads[0].id, &used->id)) {
			vq_err(vq, "Failed to write used id");
			return -EFAULT;
		}
		if (__put_user(heads[0].len, &used->len)) {
			vq_err(vq, "Failed to write used len");
			return -EFAULT;
		}
	} else if (__copy_to_user(used, heads, count * sizeof *used)) {
		vq_err(vq, "Failed to write used");
		return -EFAULT;
	}
	if (unlikely(vq->log_used)) {
		/* Make sure data is seen before log. */
		smp_wmb();
		/* Log used ring entry write. */
		log_write(vq->log_base,
			  vq->log_addr +
			   ((void __user *)used - (void __user *)vq->used),
			  count * sizeof *used);
	}
	old = vq->last_used_idx;
	new = (vq->last_used_idx += count);
	/* If the driver never bothers to signal in a very long while,
	 * used index might wrap around. If that happens, invalidate
	 * signalled_used index we stored. TODO: make sure driver
	 * signals at least once in 2^16 and remove this. */
	if (unlikely((u16)(new - vq->signalled_used) < (u16)(new - old)))
		vq->signalled_used_valid = false;
	return 0;
}
