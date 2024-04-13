static unsigned int fanout_demux_rollover(struct packet_fanout *f,
					  struct sk_buff *skb,
					  unsigned int idx, unsigned int skip,
					  unsigned int num)
{
	unsigned int i, j;

	i = j = min_t(int, f->next[idx], num - 1);
	do {
		if (i != skip && packet_rcv_has_room(pkt_sk(f->arr[i]), skb)) {
			if (i != j)
				f->next[idx] = i;
			return i;
		}
		if (++i == num)
			i = 0;
	} while (i != j);

	return idx;
}
