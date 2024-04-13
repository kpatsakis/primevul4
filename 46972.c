static void sctp_get_local_addr_list(struct net *net)
{
	struct net_device *dev;
	struct list_head *pos;
	struct sctp_af *af;

	rcu_read_lock();
	for_each_netdev_rcu(net, dev) {
		list_for_each(pos, &sctp_address_families) {
			af = list_entry(pos, struct sctp_af, list);
			af->copy_addrlist(&net->sctp.local_addr_list, dev);
		}
	}
	rcu_read_unlock();
}
