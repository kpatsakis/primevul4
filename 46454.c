static long vhost_get_vring_endian(struct vhost_virtqueue *vq, u32 idx,
				   int __user *argp)
{
	struct vhost_vring_state s = {
		.index = idx,
		.num = vq->user_be
	};

	if (copy_to_user(argp, &s, sizeof(s)))
		return -EFAULT;

	return 0;
}
