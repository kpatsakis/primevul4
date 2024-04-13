static void sctp_addr_wq_timeout_handler(unsigned long arg)
{
	struct net *net = (struct net *)arg;
	struct sctp_sockaddr_entry *addrw, *temp;
	struct sctp_sock *sp;

	spin_lock_bh(&net->sctp.addr_wq_lock);

	list_for_each_entry_safe(addrw, temp, &net->sctp.addr_waitq, list) {
		pr_debug("%s: the first ent in wq:%p is addr:%pISc for cmd:%d at "
			 "entry:%p\n", __func__, &net->sctp.addr_waitq, &addrw->a.sa,
			 addrw->state, addrw);

#if IS_ENABLED(CONFIG_IPV6)
		/* Now we send an ASCONF for each association */
		/* Note. we currently don't handle link local IPv6 addressees */
		if (addrw->a.sa.sa_family == AF_INET6) {
			struct in6_addr *in6;

			if (ipv6_addr_type(&addrw->a.v6.sin6_addr) &
			    IPV6_ADDR_LINKLOCAL)
				goto free_next;

			in6 = (struct in6_addr *)&addrw->a.v6.sin6_addr;
			if (ipv6_chk_addr(net, in6, NULL, 0) == 0 &&
			    addrw->state == SCTP_ADDR_NEW) {
				unsigned long timeo_val;

				pr_debug("%s: this is on DAD, trying %d sec "
					 "later\n", __func__,
					 SCTP_ADDRESS_TICK_DELAY);

				timeo_val = jiffies;
				timeo_val += msecs_to_jiffies(SCTP_ADDRESS_TICK_DELAY);
				mod_timer(&net->sctp.addr_wq_timer, timeo_val);
				break;
			}
		}
#endif
		list_for_each_entry(sp, &net->sctp.auto_asconf_splist, auto_asconf_list) {
			struct sock *sk;

			sk = sctp_opt2sk(sp);
			/* ignore bound-specific endpoints */
			if (!sctp_is_ep_boundall(sk))
				continue;
			bh_lock_sock(sk);
			if (sctp_asconf_mgmt(sp, addrw) < 0)
				pr_debug("%s: sctp_asconf_mgmt failed\n", __func__);
			bh_unlock_sock(sk);
		}
#if IS_ENABLED(CONFIG_IPV6)
free_next:
#endif
		list_del(&addrw->list);
		kfree(addrw);
	}
	spin_unlock_bh(&net->sctp.addr_wq_lock);
}
