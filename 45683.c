static void prb_del_retire_blk_timer(struct tpacket_kbdq_core *pkc)
{
	del_timer_sync(&pkc->retire_blk_timer);
}
