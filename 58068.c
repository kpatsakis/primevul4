static ssize_t __send_control_msg(struct ports_device *portdev, u32 port_id,
				  unsigned int event, unsigned int value)
{
	struct scatterlist sg[1];
	struct virtqueue *vq;
	unsigned int len;

	if (!use_multiport(portdev))
		return 0;

	vq = portdev->c_ovq;

	spin_lock(&portdev->c_ovq_lock);

	portdev->cpkt.id = cpu_to_virtio32(portdev->vdev, port_id);
	portdev->cpkt.event = cpu_to_virtio16(portdev->vdev, event);
	portdev->cpkt.value = cpu_to_virtio16(portdev->vdev, value);

	sg_init_one(sg, &portdev->cpkt, sizeof(struct virtio_console_control));

	if (virtqueue_add_outbuf(vq, sg, 1, &portdev->cpkt, GFP_ATOMIC) == 0) {
		virtqueue_kick(vq);
		while (!virtqueue_get_buf(vq, &len)
			&& !virtqueue_is_broken(vq))
			cpu_relax();
	}

	spin_unlock(&portdev->c_ovq_lock);
	return 0;
}
