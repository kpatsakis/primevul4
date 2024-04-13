static bool fanout_flow_is_huge(struct packet_sock *po, struct sk_buff *skb)
{
	u32 rxhash;
	int i, count = 0;

	rxhash = skb_get_hash(skb);
	for (i = 0; i < ROLLOVER_HLEN; i++)
		if (po->rollover->history[i] == rxhash)
			count++;

	po->rollover->history[prandom_u32() % ROLLOVER_HLEN] = rxhash;
	return count > (ROLLOVER_HLEN >> 1);
}
