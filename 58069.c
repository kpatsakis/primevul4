static ssize_t __send_to_port(struct port *port, struct scatterlist *sg,
			      int nents, size_t in_count,
			      void *data, bool nonblock)
{
	struct virtqueue *out_vq;
	int err;
	unsigned long flags;
	unsigned int len;

	out_vq = port->out_vq;

	spin_lock_irqsave(&port->outvq_lock, flags);

	reclaim_consumed_buffers(port);

	err = virtqueue_add_outbuf(out_vq, sg, nents, data, GFP_ATOMIC);

	/* Tell Host to go! */
	virtqueue_kick(out_vq);

	if (err) {
		in_count = 0;
		goto done;
	}

	if (out_vq->num_free == 0)
		port->outvq_full = true;

	if (nonblock)
		goto done;

	/*
	 * Wait till the host acknowledges it pushed out the data we
	 * sent.  This is done for data from the hvc_console; the tty
	 * operations are performed with spinlocks held so we can't
	 * sleep here.  An alternative would be to copy the data to a
	 * buffer and relax the spinning requirement.  The downside is
	 * we need to kmalloc a GFP_ATOMIC buffer each time the
	 * console driver writes something out.
	 */
	while (!virtqueue_get_buf(out_vq, &len)
		&& !virtqueue_is_broken(out_vq))
		cpu_relax();
done:
	spin_unlock_irqrestore(&port->outvq_lock, flags);

	port->stats.bytes_sent += in_count;
	/*
	 * We're expected to return the amount of data we wrote -- all
	 * of it
	 */
	return in_count;
}
