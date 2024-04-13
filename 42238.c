static int vq_access_ok(struct vhost_dev *d, unsigned int num,
			struct vring_desc __user *desc,
			struct vring_avail __user *avail,
			struct vring_used __user *used)
{
	size_t s = vhost_has_feature(d, VIRTIO_RING_F_EVENT_IDX) ? 2 : 0;
	return access_ok(VERIFY_READ, desc, num * sizeof *desc) &&
	       access_ok(VERIFY_READ, avail,
			 sizeof *avail + num * sizeof *avail->ring + s) &&
	       access_ok(VERIFY_WRITE, used,
			sizeof *used + num * sizeof *used->ring + s);
}
