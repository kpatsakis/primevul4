static unsigned int fanout_demux_lb(struct packet_fanout *f,
				    struct sk_buff *skb,
				    unsigned int num)
{
	unsigned int val = atomic_inc_return(&f->rr_cur);

	return val % num;
}
