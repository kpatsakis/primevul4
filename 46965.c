void sctp_addr_wq_mgmt(struct net *net, struct sctp_sockaddr_entry *addr, int cmd)
{
	struct sctp_sockaddr_entry *addrw;
	unsigned long timeo_val;

	/* first, we check if an opposite message already exist in the queue.
	 * If we found such message, it is removed.
	 * This operation is a bit stupid, but the DHCP client attaches the
	 * new address after a couple of addition and deletion of that address
	 */

	spin_lock_bh(&net->sctp.addr_wq_lock);
	/* Offsets existing events in addr_wq */
	addrw = sctp_addr_wq_lookup(net, addr);
	if (addrw) {
		if (addrw->state != cmd) {
			pr_debug("%s: offsets existing entry for %d, addr:%pISc "
				 "in wq:%p\n", __func__, addrw->state, &addrw->a.sa,
				 &net->sctp.addr_waitq);

			list_del(&addrw->list);
			kfree(addrw);
		}
		spin_unlock_bh(&net->sctp.addr_wq_lock);
		return;
	}

	/* OK, we have to add the new address to the wait queue */
	addrw = kmemdup(addr, sizeof(struct sctp_sockaddr_entry), GFP_ATOMIC);
	if (addrw == NULL) {
		spin_unlock_bh(&net->sctp.addr_wq_lock);
		return;
	}
	addrw->state = cmd;
	list_add_tail(&addrw->list, &net->sctp.addr_waitq);

	pr_debug("%s: add new entry for cmd:%d, addr:%pISc in wq:%p\n",
		 __func__, addrw->state, &addrw->a.sa, &net->sctp.addr_waitq);

	if (!timer_pending(&net->sctp.addr_wq_timer)) {
		timeo_val = jiffies;
		timeo_val += msecs_to_jiffies(SCTP_ADDRESS_TICK_DELAY);
		mod_timer(&net->sctp.addr_wq_timer, timeo_val);
	}
	spin_unlock_bh(&net->sctp.addr_wq_lock);
}
