static void vop_virtio_init_post(struct vop_vdev *vdev)
{
	struct mic_vqconfig *vqconfig = mic_vq_config(vdev->dd);
	struct vop_device *vpdev = vdev->vpdev;
	int i, used_size;

	for (i = 0; i < vdev->dd->num_vq; i++) {
		used_size = PAGE_ALIGN(sizeof(u16) * 3 +
				sizeof(struct vring_used_elem) *
				le16_to_cpu(vqconfig->num));
		if (!le64_to_cpu(vqconfig[i].used_address)) {
			dev_warn(vop_dev(vdev), "used_address zero??\n");
			continue;
		}
		vdev->vvr[i].vrh.vring.used =
			(void __force *)vpdev->hw_ops->ioremap(
			vpdev,
			le64_to_cpu(vqconfig[i].used_address),
			used_size);
	}

	vdev->dc->used_address_updated = 0;

	dev_info(vop_dev(vdev), "%s: device type %d LINKUP\n",
		 __func__, vdev->virtio_id);
}
