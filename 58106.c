static void remove_port_data(struct port *port)
{
	struct port_buffer *buf;

	spin_lock_irq(&port->inbuf_lock);
	/* Remove unused data this port might have received. */
	discard_port_data(port);
	spin_unlock_irq(&port->inbuf_lock);

	/* Remove buffers we queued up for the Host to send us data in. */
	do {
		spin_lock_irq(&port->inbuf_lock);
		buf = virtqueue_detach_unused_buf(port->in_vq);
		spin_unlock_irq(&port->inbuf_lock);
		if (buf)
			free_buf(buf, true);
	} while (buf);

	spin_lock_irq(&port->outvq_lock);
	reclaim_consumed_buffers(port);
	spin_unlock_irq(&port->outvq_lock);

	/* Free pending buffers from the out-queue. */
	do {
		spin_lock_irq(&port->outvq_lock);
		buf = virtqueue_detach_unused_buf(port->out_vq);
		spin_unlock_irq(&port->outvq_lock);
		if (buf)
			free_buf(buf, true);
	} while (buf);
}
