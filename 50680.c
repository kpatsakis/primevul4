static void packet_inc_pending(struct packet_ring_buffer *rb)
{
	this_cpu_inc(*rb->pending_refcnt);
}
