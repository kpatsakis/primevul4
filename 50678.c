static void packet_free_pending(struct packet_sock *po)
{
	free_percpu(po->tx_ring.pending_refcnt);
}
