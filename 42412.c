static int ip_vs_genl_dump_services(struct sk_buff *skb,
				    struct netlink_callback *cb)
{
	int idx = 0, i;
	int start = cb->args[0];
	struct ip_vs_service *svc;
	struct net *net = skb_sknet(skb);

	mutex_lock(&__ip_vs_mutex);
	for (i = 0; i < IP_VS_SVC_TAB_SIZE; i++) {
		list_for_each_entry(svc, &ip_vs_svc_table[i], s_list) {
			if (++idx <= start || !net_eq(svc->net, net))
				continue;
			if (ip_vs_genl_dump_service(skb, svc, cb) < 0) {
				idx--;
				goto nla_put_failure;
			}
		}
	}

	for (i = 0; i < IP_VS_SVC_TAB_SIZE; i++) {
		list_for_each_entry(svc, &ip_vs_svc_fwm_table[i], f_list) {
			if (++idx <= start || !net_eq(svc->net, net))
				continue;
			if (ip_vs_genl_dump_service(skb, svc, cb) < 0) {
				idx--;
				goto nla_put_failure;
			}
		}
	}

nla_put_failure:
	mutex_unlock(&__ip_vs_mutex);
	cb->args[0] = idx;

	return skb->len;
}
