static unsigned int fanout_demux_rnd(struct packet_fanout *f,
				     struct sk_buff *skb,
				     unsigned int num)
{
	return prandom_u32_max(num);
}
