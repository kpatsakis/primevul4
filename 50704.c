static void prb_setup_retire_blk_timer(struct packet_sock *po)
{
	struct tpacket_kbdq_core *pkc;

	pkc = GET_PBDQC_FROM_RB(&po->rx_ring);
	prb_init_blk_timer(po, pkc, prb_retire_rx_blk_timer_expired);
}
