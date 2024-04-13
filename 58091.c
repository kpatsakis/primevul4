static bool is_rproc_serial(const struct virtio_device *vdev)
{
	return is_rproc_enabled && vdev->id.device == VIRTIO_ID_RPROC_SERIAL;
}
