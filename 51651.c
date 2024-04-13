vop_query_offset(struct vop_vdev *vdev, unsigned long offset,
		 unsigned long *size, unsigned long *pa)
{
	struct vop_device *vpdev = vdev->vpdev;
	unsigned long start = MIC_DP_SIZE;
	int i;

	/*
	 * MMAP interface is as follows:
	 * offset				region
	 * 0x0					virtio device_page
	 * 0x1000				first vring
	 * 0x1000 + size of 1st vring		second vring
	 * ....
	 */
	if (!offset) {
		*pa = virt_to_phys(vpdev->hw_ops->get_dp(vpdev));
		*size = MIC_DP_SIZE;
		return 0;
	}

	for (i = 0; i < vdev->dd->num_vq; i++) {
		struct vop_vringh *vvr = &vdev->vvr[i];

		if (offset == start) {
			*pa = virt_to_phys(vvr->vring.va);
			*size = vvr->vring.len;
			return 0;
		}
		start += vvr->vring.len;
	}
	return -1;
}
