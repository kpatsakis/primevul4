static int rtnl_dump_all(struct sk_buff *skb, struct netlink_callback *cb)
{
	int idx;
	int s_idx = cb->family;

	if (s_idx == 0)
		s_idx = 1;
	for (idx = 1; idx <= RTNL_FAMILY_MAX; idx++) {
		int type = cb->nlh->nlmsg_type-RTM_BASE;
		if (idx < s_idx || idx == PF_PACKET)
			continue;
		if (rtnl_msg_handlers[idx] == NULL ||
		    rtnl_msg_handlers[idx][type].dumpit == NULL)
			continue;
		if (idx > s_idx)
			memset(&cb->args[0], 0, sizeof(cb->args));
		if (rtnl_msg_handlers[idx][type].dumpit(skb, cb))
			break;
	}
	cb->family = idx;

	return skb->len;
}
