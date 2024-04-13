static unsigned int fanout_demux_cpu(struct packet_fanout *f,
				     struct sk_buff *skb,
				     unsigned int num)
{
	return smp_processor_id() % num;
}
