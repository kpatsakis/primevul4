int vhost_add_used(struct vhost_virtqueue *vq, unsigned int head, int len)
{
	struct vring_used_elem heads = {
		cpu_to_vhost32(vq, head),
		cpu_to_vhost32(vq, len)
	};

	return vhost_add_used_n(vq, &heads, 1);
}
