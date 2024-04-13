static void vop_init_device_ctrl(struct vop_vdev *vdev,
				 struct mic_device_desc *devpage)
{
	struct mic_device_ctrl *dc;

	dc = (void *)devpage + mic_aligned_desc_size(devpage);

	dc->config_change = 0;
	dc->guest_ack = 0;
	dc->vdev_reset = 0;
	dc->host_ack = 0;
	dc->used_address_updated = 0;
	dc->c2h_vdev_db = -1;
	dc->h2c_vdev_db = -1;
	vdev->dc = dc;
}
