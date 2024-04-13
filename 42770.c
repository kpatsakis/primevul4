u16 __skb_tx_hash(const struct net_device *dev, const struct sk_buff *skb,
		  unsigned int num_tx_queues)
{
	u32 hash;

	if (skb_rx_queue_recorded(skb)) {
		hash = skb_get_rx_queue(skb);
		while (unlikely(hash >= num_tx_queues))
			hash -= num_tx_queues;
		return hash;
	}

	if (skb->sk && skb->sk->sk_hash)
		hash = skb->sk->sk_hash;
	else
		hash = (__force u16) skb->protocol ^ skb->rxhash;
	hash = jhash_1word(hash, hashrnd);

	return (u16) (((u64) hash * num_tx_queues) >> 32);
}
