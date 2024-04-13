static int ip_vs_genl_dump_dests(struct sk_buff *skb,
				 struct netlink_callback *cb)
{
	int idx = 0;
	int start = cb->args[0];
	struct ip_vs_service *svc;
	struct ip_vs_dest *dest;
	struct nlattr *attrs[IPVS_CMD_ATTR_MAX + 1];
	struct net *net = skb_sknet(skb);

	mutex_lock(&__ip_vs_mutex);

	/* Try to find the service for which to dump destinations */
	if (nlmsg_parse(cb->nlh, GENL_HDRLEN, attrs,
			IPVS_CMD_ATTR_MAX, ip_vs_cmd_policy))
		goto out_err;


	svc = ip_vs_genl_find_service(net, attrs[IPVS_CMD_ATTR_SERVICE]);
	if (IS_ERR(svc) || svc == NULL)
		goto out_err;

	/* Dump the destinations */
	list_for_each_entry(dest, &svc->destinations, n_list) {
		if (++idx <= start)
			continue;
		if (ip_vs_genl_dump_dest(skb, dest, cb) < 0) {
			idx--;
			goto nla_put_failure;
		}
	}

nla_put_failure:
	cb->args[0] = idx;

out_err:
	mutex_unlock(&__ip_vs_mutex);

	return skb->len;
}
