static int dccp_to_nlattr(struct sk_buff *skb, struct nlattr *nla,
			  struct nf_conn *ct)
{
	struct nlattr *nest_parms;

	spin_lock_bh(&ct->lock);
	nest_parms = nla_nest_start(skb, CTA_PROTOINFO_DCCP | NLA_F_NESTED);
	if (!nest_parms)
		goto nla_put_failure;
	if (nla_put_u8(skb, CTA_PROTOINFO_DCCP_STATE, ct->proto.dccp.state) ||
	    nla_put_u8(skb, CTA_PROTOINFO_DCCP_ROLE,
		       ct->proto.dccp.role[IP_CT_DIR_ORIGINAL]) ||
	    nla_put_be64(skb, CTA_PROTOINFO_DCCP_HANDSHAKE_SEQ,
			 cpu_to_be64(ct->proto.dccp.handshake_seq)))
		goto nla_put_failure;
	nla_nest_end(skb, nest_parms);
	spin_unlock_bh(&ct->lock);
	return 0;

nla_put_failure:
	spin_unlock_bh(&ct->lock);
	return -1;
}
