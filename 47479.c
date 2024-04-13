static void ndisc_solicit(struct neighbour *neigh, struct sk_buff *skb)
{
	struct in6_addr *saddr = NULL;
	struct in6_addr mcaddr;
	struct net_device *dev = neigh->dev;
	struct in6_addr *target = (struct in6_addr *)&neigh->primary_key;
	int probes = atomic_read(&neigh->probes);

	if (skb && ipv6_chk_addr_and_flags(dev_net(dev), &ipv6_hdr(skb)->saddr,
					   dev, 1,
					   IFA_F_TENTATIVE|IFA_F_OPTIMISTIC))
		saddr = &ipv6_hdr(skb)->saddr;
	probes -= NEIGH_VAR(neigh->parms, UCAST_PROBES);
	if (probes < 0) {
		if (!(neigh->nud_state & NUD_VALID)) {
			ND_PRINTK(1, dbg,
				  "%s: trying to ucast probe in NUD_INVALID: %pI6\n",
				  __func__, target);
		}
		ndisc_send_ns(dev, neigh, target, target, saddr);
	} else if ((probes -= NEIGH_VAR(neigh->parms, APP_PROBES)) < 0) {
		neigh_app_ns(neigh);
	} else {
		addrconf_addr_solict_mult(target, &mcaddr);
		ndisc_send_ns(dev, NULL, target, &mcaddr, saddr);
	}
}
