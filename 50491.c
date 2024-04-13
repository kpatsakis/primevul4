static int vfio_msi_set_block(struct vfio_pci_device *vdev, unsigned start,
			      unsigned count, int32_t *fds, bool msix)
{
	int i, j, ret = 0;

	if (start >= vdev->num_ctx || start + count > vdev->num_ctx)
		return -EINVAL;

	for (i = 0, j = start; i < count && !ret; i++, j++) {
		int fd = fds ? fds[i] : -1;
		ret = vfio_msi_set_vector_signal(vdev, j, fd, msix);
	}

	if (ret) {
		for (--j; j >= (int)start; j--)
			vfio_msi_set_vector_signal(vdev, j, -1, msix);
	}

	return ret;
}
