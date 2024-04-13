static void vhost_init_is_le(struct vhost_virtqueue *vq)
{
	/* Note for legacy virtio: user_be is initialized at reset time
	 * according to the host endianness. If userspace does not set an
	 * explicit endianness, the default behavior is native endian, as
	 * expected by legacy virtio.
	 */
	vq->is_le = vhost_has_feature(vq, VIRTIO_F_VERSION_1) || !vq->user_be;
}
