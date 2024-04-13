static void remove_controlq_data(struct ports_device *portdev)
{
	struct port_buffer *buf;
	unsigned int len;

	if (!use_multiport(portdev))
		return;

	while ((buf = virtqueue_get_buf(portdev->c_ivq, &len)))
		free_buf(buf, true);

	while ((buf = virtqueue_detach_unused_buf(portdev->c_ivq)))
		free_buf(buf, true);
}
