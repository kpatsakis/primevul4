static int ip_vs_genl_fill_service(struct sk_buff *skb,
				   struct ip_vs_service *svc)
{
	struct nlattr *nl_service;
	struct ip_vs_flags flags = { .flags = svc->flags,
				     .mask = ~0 };

	nl_service = nla_nest_start(skb, IPVS_CMD_ATTR_SERVICE);
	if (!nl_service)
		return -EMSGSIZE;

	if (nla_put_u16(skb, IPVS_SVC_ATTR_AF, svc->af))
		goto nla_put_failure;
	if (svc->fwmark) {
		if (nla_put_u32(skb, IPVS_SVC_ATTR_FWMARK, svc->fwmark))
			goto nla_put_failure;
	} else {
		if (nla_put_u16(skb, IPVS_SVC_ATTR_PROTOCOL, svc->protocol) ||
		    nla_put(skb, IPVS_SVC_ATTR_ADDR, sizeof(svc->addr), &svc->addr) ||
		    nla_put_u16(skb, IPVS_SVC_ATTR_PORT, svc->port))
			goto nla_put_failure;
	}

	if (nla_put_string(skb, IPVS_SVC_ATTR_SCHED_NAME, svc->scheduler->name) ||
	    (svc->pe &&
	     nla_put_string(skb, IPVS_SVC_ATTR_PE_NAME, svc->pe->name)) ||
	    nla_put(skb, IPVS_SVC_ATTR_FLAGS, sizeof(flags), &flags) ||
	    nla_put_u32(skb, IPVS_SVC_ATTR_TIMEOUT, svc->timeout / HZ) ||
	    nla_put_u32(skb, IPVS_SVC_ATTR_NETMASK, svc->netmask))
		goto nla_put_failure;
	if (ip_vs_genl_fill_stats(skb, IPVS_SVC_ATTR_STATS, &svc->stats))
		goto nla_put_failure;

	nla_nest_end(skb, nl_service);

	return 0;

nla_put_failure:
	nla_nest_cancel(skb, nl_service);
	return -EMSGSIZE;
}
