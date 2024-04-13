static int vhost_update_used_flags(struct vhost_virtqueue *vq)
{
	void __user *used;
	if (__put_user(cpu_to_vhost16(vq, vq->used_flags), &vq->used->flags) < 0)
		return -EFAULT;
	if (unlikely(vq->log_used)) {
		/* Make sure the flag is seen before log. */
		smp_wmb();
		/* Log used flag write. */
		used = &vq->used->flags;
		log_write(vq->log_base, vq->log_addr +
			  (used - (void __user *)vq->used),
			  sizeof vq->used->flags);
		if (vq->log_ctx)
			eventfd_signal(vq->log_ctx, 1);
	}
	return 0;
}
