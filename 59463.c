vc4_wait_seqno_ioctl(struct drm_device *dev, void *data,
		     struct drm_file *file_priv)
{
	struct drm_vc4_wait_seqno *args = data;

	return vc4_wait_for_seqno_ioctl_helper(dev, args->seqno,
					       &args->timeout_ns);
}
