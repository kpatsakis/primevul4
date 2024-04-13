static unsigned int skb_ts_get_next_block(unsigned int offset, const u8 **text,
					  struct ts_config *conf,
					  struct ts_state *state)
{
	return skb_seq_read(offset, text, TS_SKB_CB(state));
}
