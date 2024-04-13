vc4_wait_for_seqno_ioctl_helper(struct drm_device *dev,
				uint64_t seqno,
				uint64_t *timeout_ns)
{
	unsigned long start = jiffies;
	int ret = vc4_wait_for_seqno(dev, seqno, *timeout_ns, true);

	if ((ret == -EINTR || ret == -ERESTARTSYS) && *timeout_ns != ~0ull) {
		uint64_t delta = jiffies_to_nsecs(jiffies - start);

		if (*timeout_ns >= delta)
			*timeout_ns -= delta;
	}

	return ret;
}
