static void sctp_free_local_addr_list(struct net *net)
{
	struct sctp_sockaddr_entry *addr;
	struct list_head *pos, *temp;

	list_for_each_safe(pos, temp, &net->sctp.local_addr_list) {
		addr = list_entry(pos, struct sctp_sockaddr_entry, list);
		list_del(pos);
		kfree(addr);
	}
}
