static int ip_vs_genl_get_cmd(struct sk_buff *skb, struct genl_info *info)
{
	struct sk_buff *msg;
	void *reply;
	int ret, cmd, reply_cmd;
	struct net *net;

	net = skb_sknet(skb);
	cmd = info->genlhdr->cmd;

	if (cmd == IPVS_CMD_GET_SERVICE)
		reply_cmd = IPVS_CMD_NEW_SERVICE;
	else if (cmd == IPVS_CMD_GET_INFO)
		reply_cmd = IPVS_CMD_SET_INFO;
	else if (cmd == IPVS_CMD_GET_CONFIG)
		reply_cmd = IPVS_CMD_SET_CONFIG;
	else {
		pr_err("unknown Generic Netlink command\n");
		return -EINVAL;
	}

	msg = nlmsg_new(NLMSG_DEFAULT_SIZE, GFP_KERNEL);
	if (!msg)
		return -ENOMEM;

	mutex_lock(&__ip_vs_mutex);

	reply = genlmsg_put_reply(msg, info, &ip_vs_genl_family, 0, reply_cmd);
	if (reply == NULL)
		goto nla_put_failure;

	switch (cmd) {
	case IPVS_CMD_GET_SERVICE:
	{
		struct ip_vs_service *svc;

		svc = ip_vs_genl_find_service(net,
					      info->attrs[IPVS_CMD_ATTR_SERVICE]);
		if (IS_ERR(svc)) {
			ret = PTR_ERR(svc);
			goto out_err;
		} else if (svc) {
			ret = ip_vs_genl_fill_service(msg, svc);
			if (ret)
				goto nla_put_failure;
		} else {
			ret = -ESRCH;
			goto out_err;
		}

		break;
	}

	case IPVS_CMD_GET_CONFIG:
	{
		struct ip_vs_timeout_user t;

		__ip_vs_get_timeouts(net, &t);
#ifdef CONFIG_IP_VS_PROTO_TCP
		if (nla_put_u32(msg, IPVS_CMD_ATTR_TIMEOUT_TCP,
				t.tcp_timeout) ||
		    nla_put_u32(msg, IPVS_CMD_ATTR_TIMEOUT_TCP_FIN,
				t.tcp_fin_timeout))
			goto nla_put_failure;
#endif
#ifdef CONFIG_IP_VS_PROTO_UDP
		if (nla_put_u32(msg, IPVS_CMD_ATTR_TIMEOUT_UDP, t.udp_timeout))
			goto nla_put_failure;
#endif

		break;
	}

	case IPVS_CMD_GET_INFO:
		if (nla_put_u32(msg, IPVS_INFO_ATTR_VERSION,
				IP_VS_VERSION_CODE) ||
		    nla_put_u32(msg, IPVS_INFO_ATTR_CONN_TAB_SIZE,
				ip_vs_conn_tab_size))
			goto nla_put_failure;
		break;
	}

	genlmsg_end(msg, reply);
	ret = genlmsg_reply(msg, info);
	goto out;

nla_put_failure:
	pr_err("not enough space in Netlink message\n");
	ret = -EMSGSIZE;

out_err:
	nlmsg_free(msg);
out:
	mutex_unlock(&__ip_vs_mutex);

	return ret;
}
