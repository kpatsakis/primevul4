static int packet_alloc_pending(struct packet_sock *po)
{
	po->rx_ring.pending_refcnt = NULL;

	po->tx_ring.pending_refcnt = alloc_percpu(unsigned int);
	if (unlikely(po->tx_ring.pending_refcnt == NULL))
		return -ENOBUFS;

	return 0;
}
