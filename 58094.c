static void out_intr(struct virtqueue *vq)
{
	struct port *port;

	port = find_port_by_vq(vq->vdev->priv, vq);
	if (!port)
		return;

	wake_up_interruptible(&port->waitqueue);
}
