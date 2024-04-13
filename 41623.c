SCTP_STATIC void sctp_destroy_sock(struct sock *sk)
{
	struct sctp_sock *sp;

	SCTP_DEBUG_PRINTK("sctp_destroy_sock(sk: %p)\n", sk);

	/* Release our hold on the endpoint. */
	sp = sctp_sk(sk);
	if (sp->do_auto_asconf) {
		sp->do_auto_asconf = 0;
		list_del(&sp->auto_asconf_list);
	}
	sctp_endpoint_free(sp->ep);
	local_bh_disable();
	percpu_counter_dec(&sctp_sockets_allocated);
	sock_prot_inuse_add(sock_net(sk), sk->sk_prot, -1);
	local_bh_enable();
}
