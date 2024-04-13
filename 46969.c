static void sctp_free_addr_wq(struct net *net)
{
	struct sctp_sockaddr_entry *addrw;
	struct sctp_sockaddr_entry *temp;

	spin_lock_bh(&net->sctp.addr_wq_lock);
	del_timer(&net->sctp.addr_wq_timer);
	list_for_each_entry_safe(addrw, temp, &net->sctp.addr_waitq, list) {
		list_del(&addrw->list);
		kfree(addrw);
	}
	spin_unlock_bh(&net->sctp.addr_wq_lock);
}
