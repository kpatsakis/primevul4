static void init_prb_bdqc(struct packet_sock *po,
			struct packet_ring_buffer *rb,
			struct pgv *pg_vec,
			union tpacket_req_u *req_u)
{
	struct tpacket_kbdq_core *p1 = GET_PBDQC_FROM_RB(rb);
	struct tpacket_block_desc *pbd;

	memset(p1, 0x0, sizeof(*p1));

	p1->knxt_seq_num = 1;
	p1->pkbdq = pg_vec;
	pbd = (struct tpacket_block_desc *)pg_vec[0].buffer;
	p1->pkblk_start	= pg_vec[0].buffer;
	p1->kblk_size = req_u->req3.tp_block_size;
	p1->knum_blocks	= req_u->req3.tp_block_nr;
	p1->hdrlen = po->tp_hdrlen;
	p1->version = po->tp_version;
	p1->last_kactive_blk_num = 0;
	po->stats.stats3.tp_freeze_q_cnt = 0;
	if (req_u->req3.tp_retire_blk_tov)
		p1->retire_blk_tov = req_u->req3.tp_retire_blk_tov;
	else
		p1->retire_blk_tov = prb_calc_retire_blk_tmo(po,
						req_u->req3.tp_block_size);
	p1->tov_in_jiffies = msecs_to_jiffies(p1->retire_blk_tov);
	p1->blk_sizeof_priv = req_u->req3.tp_sizeof_priv;

	p1->max_frame_len = p1->kblk_size - BLK_PLUS_PRIV(p1->blk_sizeof_priv);
	prb_init_ft_ops(p1, req_u);
	prb_setup_retire_blk_timer(po);
	prb_open_block(p1, pbd);
}
