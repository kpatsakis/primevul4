int vhost_init_used(struct vhost_virtqueue *vq)
{
	int r;
	if (!vq->private_data)
		return 0;

	r = vhost_update_used_flags(vq);
	if (r)
		return r;
	vq->signalled_used_valid = false;
	return get_user(vq->last_used_idx, &vq->used->idx);
}
