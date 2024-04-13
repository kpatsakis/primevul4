int vhost_init_used(struct vhost_virtqueue *vq)
{
	__virtio16 last_used_idx;
	int r;
	if (!vq->private_data) {
		vq->is_le = virtio_legacy_is_little_endian();
		return 0;
	}

	vhost_init_is_le(vq);

	r = vhost_update_used_flags(vq);
	if (r)
		return r;
	vq->signalled_used_valid = false;
	if (!access_ok(VERIFY_READ, &vq->used->idx, sizeof vq->used->idx))
		return -EFAULT;
	r = __get_user(last_used_idx, &vq->used->idx);
	if (r)
		return r;
	vq->last_used_idx = vhost16_to_cpu(vq, last_used_idx);
	return 0;
}
