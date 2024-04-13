static int prb_curr_blk_in_use(struct tpacket_block_desc *pbd)
{
	return TP_STATUS_USER & BLOCK_STATUS(pbd);
}
