static bool packet_rcv_has_room(struct packet_sock *po, struct sk_buff *skb)
{
	struct sock *sk = &po->sk;
	bool has_room;

	if (po->prot_hook.func != tpacket_rcv)
		return (atomic_read(&sk->sk_rmem_alloc) + skb->truesize)
			<= sk->sk_rcvbuf;

	spin_lock(&sk->sk_receive_queue.lock);
	if (po->tp_version == TPACKET_V3)
		has_room = prb_lookup_block(po, &po->rx_ring,
					    po->rx_ring.prb_bdqc.kactive_blk_num,
					    TP_STATUS_KERNEL);
	else
		has_room = packet_lookup_frame(po, &po->rx_ring,
					       po->rx_ring.head,
					       TP_STATUS_KERNEL);
	spin_unlock(&sk->sk_receive_queue.lock);

	return has_room;
}
