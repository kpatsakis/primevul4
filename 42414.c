static int ip_vs_genl_fill_dest(struct sk_buff *skb, struct ip_vs_dest *dest)
{
	struct nlattr *nl_dest;

	nl_dest = nla_nest_start(skb, IPVS_CMD_ATTR_DEST);
	if (!nl_dest)
		return -EMSGSIZE;

	if (nla_put(skb, IPVS_DEST_ATTR_ADDR, sizeof(dest->addr), &dest->addr) ||
	    nla_put_u16(skb, IPVS_DEST_ATTR_PORT, dest->port) ||
	    nla_put_u32(skb, IPVS_DEST_ATTR_FWD_METHOD,
			(atomic_read(&dest->conn_flags) &
			 IP_VS_CONN_F_FWD_MASK)) ||
	    nla_put_u32(skb, IPVS_DEST_ATTR_WEIGHT,
			atomic_read(&dest->weight)) ||
	    nla_put_u32(skb, IPVS_DEST_ATTR_U_THRESH, dest->u_threshold) ||
	    nla_put_u32(skb, IPVS_DEST_ATTR_L_THRESH, dest->l_threshold) ||
	    nla_put_u32(skb, IPVS_DEST_ATTR_ACTIVE_CONNS,
			atomic_read(&dest->activeconns)) ||
	    nla_put_u32(skb, IPVS_DEST_ATTR_INACT_CONNS,
			atomic_read(&dest->inactconns)) ||
	    nla_put_u32(skb, IPVS_DEST_ATTR_PERSIST_CONNS,
			atomic_read(&dest->persistconns)))
		goto nla_put_failure;
	if (ip_vs_genl_fill_stats(skb, IPVS_DEST_ATTR_STATS, &dest->stats))
		goto nla_put_failure;

	nla_nest_end(skb, nl_dest);

	return 0;

nla_put_failure:
	nla_nest_cancel(skb, nl_dest);
	return -EMSGSIZE;
}
