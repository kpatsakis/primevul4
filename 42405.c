static int ip_vs_dst_event(struct notifier_block *this, unsigned long event,
			    void *ptr)
{
	struct net_device *dev = ptr;
	struct net *net = dev_net(dev);
	struct netns_ipvs *ipvs = net_ipvs(net);
	struct ip_vs_service *svc;
	struct ip_vs_dest *dest;
	unsigned int idx;

	if (event != NETDEV_UNREGISTER || !ipvs)
		return NOTIFY_DONE;
	IP_VS_DBG(3, "%s() dev=%s\n", __func__, dev->name);
	EnterFunction(2);
	mutex_lock(&__ip_vs_mutex);
	for (idx = 0; idx < IP_VS_SVC_TAB_SIZE; idx++) {
		list_for_each_entry(svc, &ip_vs_svc_table[idx], s_list) {
			if (net_eq(svc->net, net)) {
				list_for_each_entry(dest, &svc->destinations,
						    n_list) {
					__ip_vs_dev_reset(dest, dev);
				}
			}
		}

		list_for_each_entry(svc, &ip_vs_svc_fwm_table[idx], f_list) {
			if (net_eq(svc->net, net)) {
				list_for_each_entry(dest, &svc->destinations,
						    n_list) {
					__ip_vs_dev_reset(dest, dev);
				}
			}

		}
	}

	list_for_each_entry(dest, &ipvs->dest_trash, n_list) {
		__ip_vs_dev_reset(dest, dev);
	}
	mutex_unlock(&__ip_vs_mutex);
	LeaveFunction(2);
	return NOTIFY_DONE;
}
