static void vhost_vq_reset_user_be(struct vhost_virtqueue *vq)
{
	vq->user_be = !virtio_legacy_is_little_endian();
}
