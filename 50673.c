static void packet_dec_pending(struct packet_ring_buffer *rb)
{
	this_cpu_dec(*rb->pending_refcnt);
}
