int netif_receive_skb(struct sk_buff *skb)
{
	if (netdev_tstamp_prequeue)
		net_timestamp_check(skb);

	if (skb_defer_rx_timestamp(skb))
		return NET_RX_SUCCESS;

#ifdef CONFIG_RPS
	{
		struct rps_dev_flow voidflow, *rflow = &voidflow;
		int cpu, ret;

		rcu_read_lock();

		cpu = get_rps_cpu(skb->dev, skb, &rflow);

		if (cpu >= 0) {
			ret = enqueue_to_backlog(skb, cpu, &rflow->last_qtail);
			rcu_read_unlock();
		} else {
			rcu_read_unlock();
			ret = __netif_receive_skb(skb);
		}

		return ret;
	}
#else
	return __netif_receive_skb(skb);
#endif
}
