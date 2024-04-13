static void sctp_v4_copy_addrlist(struct list_head *addrlist,
				  struct net_device *dev)
{
	struct in_device *in_dev;
	struct in_ifaddr *ifa;
	struct sctp_sockaddr_entry *addr;

	rcu_read_lock();
	if ((in_dev = __in_dev_get_rcu(dev)) == NULL) {
		rcu_read_unlock();
		return;
	}

	for (ifa = in_dev->ifa_list; ifa; ifa = ifa->ifa_next) {
		/* Add the address to the local list.  */
		addr = kzalloc(sizeof(*addr), GFP_ATOMIC);
		if (addr) {
			addr->a.v4.sin_family = AF_INET;
			addr->a.v4.sin_port = 0;
			addr->a.v4.sin_addr.s_addr = ifa->ifa_local;
			addr->valid = 1;
			INIT_LIST_HEAD(&addr->list);
			list_add_tail(&addr->list, addrlist);
		}
	}

	rcu_read_unlock();
}
