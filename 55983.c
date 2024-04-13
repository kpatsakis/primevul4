int ip_route_input_rcu(struct sk_buff *skb, __be32 daddr, __be32 saddr,
		       u8 tos, struct net_device *dev, struct fib_result *res)
{
	/* Multicast recognition logic is moved from route cache to here.
	   The problem was that too many Ethernet cards have broken/missing
	   hardware multicast filters :-( As result the host on multicasting
	   network acquires a lot of useless route cache entries, sort of
	   SDR messages from all the world. Now we try to get rid of them.
	   Really, provided software IP multicast filter is organized
	   reasonably (at least, hashed), it does not result in a slowdown
	   comparing with route cache reject entries.
	   Note, that multicast routers are not affected, because
	   route cache entry is created eventually.
	 */
	if (ipv4_is_multicast(daddr)) {
		struct in_device *in_dev = __in_dev_get_rcu(dev);
		int our = 0;
		int err = -EINVAL;

		if (in_dev)
			our = ip_check_mc_rcu(in_dev, daddr, saddr,
					      ip_hdr(skb)->protocol);

		/* check l3 master if no match yet */
		if ((!in_dev || !our) && netif_is_l3_slave(dev)) {
			struct in_device *l3_in_dev;

			l3_in_dev = __in_dev_get_rcu(skb->dev);
			if (l3_in_dev)
				our = ip_check_mc_rcu(l3_in_dev, daddr, saddr,
						      ip_hdr(skb)->protocol);
		}

		if (our
#ifdef CONFIG_IP_MROUTE
			||
		    (!ipv4_is_local_multicast(daddr) &&
		     IN_DEV_MFORWARD(in_dev))
#endif
		   ) {
			err = ip_route_input_mc(skb, daddr, saddr,
						tos, dev, our);
		}
		return err;
	}

	return ip_route_input_slow(skb, daddr, saddr, tos, dev, res);
}
