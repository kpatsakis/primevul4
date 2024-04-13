static unsigned int fanout_demux_qm(struct packet_fanout *f,
				    struct sk_buff *skb,
				    unsigned int num)
{
	return skb_get_queue_mapping(skb) % num;
}
