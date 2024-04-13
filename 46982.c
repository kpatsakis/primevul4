static int sctp_inetaddr_event(struct notifier_block *this, unsigned long ev,
			       void *ptr)
{
	struct in_ifaddr *ifa = (struct in_ifaddr *)ptr;
	struct sctp_sockaddr_entry *addr = NULL;
	struct sctp_sockaddr_entry *temp;
	struct net *net = dev_net(ifa->ifa_dev->dev);
	int found = 0;

	switch (ev) {
	case NETDEV_UP:
		addr = kmalloc(sizeof(struct sctp_sockaddr_entry), GFP_ATOMIC);
		if (addr) {
			addr->a.v4.sin_family = AF_INET;
			addr->a.v4.sin_port = 0;
			addr->a.v4.sin_addr.s_addr = ifa->ifa_local;
			addr->valid = 1;
			spin_lock_bh(&net->sctp.local_addr_lock);
			list_add_tail_rcu(&addr->list, &net->sctp.local_addr_list);
			sctp_addr_wq_mgmt(net, addr, SCTP_ADDR_NEW);
			spin_unlock_bh(&net->sctp.local_addr_lock);
		}
		break;
	case NETDEV_DOWN:
		spin_lock_bh(&net->sctp.local_addr_lock);
		list_for_each_entry_safe(addr, temp,
					&net->sctp.local_addr_list, list) {
			if (addr->a.sa.sa_family == AF_INET &&
					addr->a.v4.sin_addr.s_addr ==
					ifa->ifa_local) {
				sctp_addr_wq_mgmt(net, addr, SCTP_ADDR_DEL);
				found = 1;
				addr->valid = 0;
				list_del_rcu(&addr->list);
				break;
			}
		}
		spin_unlock_bh(&net->sctp.local_addr_lock);
		if (found)
			kfree_rcu(addr, rcu);
		break;
	}

	return NOTIFY_DONE;
}
