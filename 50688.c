static unsigned int packet_read_pending(const struct packet_ring_buffer *rb)
{
	unsigned int refcnt = 0;
	int cpu;

	/* We don't use pending refcount in rx_ring. */
	if (rb->pending_refcnt == NULL)
		return 0;

	for_each_possible_cpu(cpu)
		refcnt += *per_cpu_ptr(rb->pending_refcnt, cpu);

	return refcnt;
}
