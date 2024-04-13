static void *netlink_seq_next(struct seq_file *seq, void *v, loff_t *pos)
{
	struct sock *s;
	struct nl_seq_iter *iter;
	struct net *net;
	int i, j;

	++*pos;

	if (v == SEQ_START_TOKEN)
		return netlink_seq_socket_idx(seq, 0);

	net = seq_file_net(seq);
	iter = seq->private;
	s = v;
	do {
		s = sk_next(s);
	} while (s && !nl_table[s->sk_protocol].compare(net, s));
	if (s)
		return s;

	i = iter->link;
	j = iter->hash_idx + 1;

	do {
		struct nl_portid_hash *hash = &nl_table[i].hash;

		for (; j <= hash->mask; j++) {
			s = sk_head(&hash->table[j]);

			while (s && !nl_table[s->sk_protocol].compare(net, s))
				s = sk_next(s);
			if (s) {
				iter->link = i;
				iter->hash_idx = j;
				return s;
			}
		}

		j = 0;
	} while (++i < MAX_LINKS);

	return NULL;
}
