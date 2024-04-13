static int ip_vs_genl_set_daemon(struct sk_buff *skb, struct genl_info *info)
{
	int ret = 0, cmd;
	struct net *net;
	struct netns_ipvs *ipvs;

	net = skb_sknet(skb);
	ipvs = net_ipvs(net);
	cmd = info->genlhdr->cmd;

	if (cmd == IPVS_CMD_NEW_DAEMON || cmd == IPVS_CMD_DEL_DAEMON) {
		struct nlattr *daemon_attrs[IPVS_DAEMON_ATTR_MAX + 1];

		mutex_lock(&ipvs->sync_mutex);
		if (!info->attrs[IPVS_CMD_ATTR_DAEMON] ||
		    nla_parse_nested(daemon_attrs, IPVS_DAEMON_ATTR_MAX,
				     info->attrs[IPVS_CMD_ATTR_DAEMON],
				     ip_vs_daemon_policy)) {
			ret = -EINVAL;
			goto out;
		}

		if (cmd == IPVS_CMD_NEW_DAEMON)
			ret = ip_vs_genl_new_daemon(net, daemon_attrs);
		else
			ret = ip_vs_genl_del_daemon(net, daemon_attrs);
out:
		mutex_unlock(&ipvs->sync_mutex);
	}
	return ret;
}
