static unsigned int fill_queue(struct virtqueue *vq, spinlock_t *lock)
{
	struct port_buffer *buf;
	unsigned int nr_added_bufs;
	int ret;

	nr_added_bufs = 0;
	do {
		buf = alloc_buf(vq, PAGE_SIZE, 0);
		if (!buf)
			break;

		spin_lock_irq(lock);
		ret = add_inbuf(vq, buf);
		if (ret < 0) {
			spin_unlock_irq(lock);
			free_buf(buf, true);
			break;
		}
		nr_added_bufs++;
		spin_unlock_irq(lock);
	} while (ret > 0);

	return nr_added_bufs;
}
