static void skb_ts_finish(struct ts_config *conf, struct ts_state *state)
{
	skb_abort_seq_read(TS_SKB_CB(state));
}
