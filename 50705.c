static void prb_shutdown_retire_blk_timer(struct packet_sock *po,
		struct sk_buff_head *rb_queue)
{
	struct tpacket_kbdq_core *pkc;

	pkc = GET_PBDQC_FROM_RB(&po->rx_ring);

	spin_lock_bh(&rb_queue->lock);
	pkc->delete_blk_timer = 1;
	spin_unlock_bh(&rb_queue->lock);

	prb_del_retire_blk_timer(pkc);
}
