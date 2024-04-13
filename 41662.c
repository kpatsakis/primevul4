SCTP_STATIC int sctp_init_sock(struct sock *sk)
{
	struct net *net = sock_net(sk);
	struct sctp_endpoint *ep;
	struct sctp_sock *sp;

	SCTP_DEBUG_PRINTK("sctp_init_sock(sk: %p)\n", sk);

	sp = sctp_sk(sk);

	/* Initialize the SCTP per socket area.  */
	switch (sk->sk_type) {
	case SOCK_SEQPACKET:
		sp->type = SCTP_SOCKET_UDP;
		break;
	case SOCK_STREAM:
		sp->type = SCTP_SOCKET_TCP;
		break;
	default:
		return -ESOCKTNOSUPPORT;
	}

	/* Initialize default send parameters. These parameters can be
	 * modified with the SCTP_DEFAULT_SEND_PARAM socket option.
	 */
	sp->default_stream = 0;
	sp->default_ppid = 0;
	sp->default_flags = 0;
	sp->default_context = 0;
	sp->default_timetolive = 0;

	sp->default_rcv_context = 0;
	sp->max_burst = net->sctp.max_burst;

	sp->sctp_hmac_alg = net->sctp.sctp_hmac_alg;

	/* Initialize default setup parameters. These parameters
	 * can be modified with the SCTP_INITMSG socket option or
	 * overridden by the SCTP_INIT CMSG.
	 */
	sp->initmsg.sinit_num_ostreams   = sctp_max_outstreams;
	sp->initmsg.sinit_max_instreams  = sctp_max_instreams;
	sp->initmsg.sinit_max_attempts   = net->sctp.max_retrans_init;
	sp->initmsg.sinit_max_init_timeo = net->sctp.rto_max;

	/* Initialize default RTO related parameters.  These parameters can
	 * be modified for with the SCTP_RTOINFO socket option.
	 */
	sp->rtoinfo.srto_initial = net->sctp.rto_initial;
	sp->rtoinfo.srto_max     = net->sctp.rto_max;
	sp->rtoinfo.srto_min     = net->sctp.rto_min;

	/* Initialize default association related parameters. These parameters
	 * can be modified with the SCTP_ASSOCINFO socket option.
	 */
	sp->assocparams.sasoc_asocmaxrxt = net->sctp.max_retrans_association;
	sp->assocparams.sasoc_number_peer_destinations = 0;
	sp->assocparams.sasoc_peer_rwnd = 0;
	sp->assocparams.sasoc_local_rwnd = 0;
	sp->assocparams.sasoc_cookie_life = net->sctp.valid_cookie_life;

	/* Initialize default event subscriptions. By default, all the
	 * options are off.
	 */
	memset(&sp->subscribe, 0, sizeof(struct sctp_event_subscribe));

	/* Default Peer Address Parameters.  These defaults can
	 * be modified via SCTP_PEER_ADDR_PARAMS
	 */
	sp->hbinterval  = net->sctp.hb_interval;
	sp->pathmaxrxt  = net->sctp.max_retrans_path;
	sp->pathmtu     = 0; // allow default discovery
	sp->sackdelay   = net->sctp.sack_timeout;
	sp->sackfreq	= 2;
	sp->param_flags = SPP_HB_ENABLE |
			  SPP_PMTUD_ENABLE |
			  SPP_SACKDELAY_ENABLE;

	/* If enabled no SCTP message fragmentation will be performed.
	 * Configure through SCTP_DISABLE_FRAGMENTS socket option.
	 */
	sp->disable_fragments = 0;

	/* Enable Nagle algorithm by default.  */
	sp->nodelay           = 0;

	/* Enable by default. */
	sp->v4mapped          = 1;

	/* Auto-close idle associations after the configured
	 * number of seconds.  A value of 0 disables this
	 * feature.  Configure through the SCTP_AUTOCLOSE socket option,
	 * for UDP-style sockets only.
	 */
	sp->autoclose         = 0;

	/* User specified fragmentation limit. */
	sp->user_frag         = 0;

	sp->adaptation_ind = 0;

	sp->pf = sctp_get_pf_specific(sk->sk_family);

	/* Control variables for partial data delivery. */
	atomic_set(&sp->pd_mode, 0);
	skb_queue_head_init(&sp->pd_lobby);
	sp->frag_interleave = 0;

	/* Create a per socket endpoint structure.  Even if we
	 * change the data structure relationships, this may still
	 * be useful for storing pre-connect address information.
	 */
	ep = sctp_endpoint_new(sk, GFP_KERNEL);
	if (!ep)
		return -ENOMEM;

	sp->ep = ep;
	sp->hmac = NULL;

	SCTP_DBG_OBJCNT_INC(sock);

	local_bh_disable();
	percpu_counter_inc(&sctp_sockets_allocated);
	sock_prot_inuse_add(net, sk->sk_prot, 1);
	if (net->sctp.default_auto_asconf) {
		list_add_tail(&sp->auto_asconf_list,
		    &net->sctp.auto_asconf_splist);
		sp->do_auto_asconf = 1;
	} else
		sp->do_auto_asconf = 0;
	local_bh_enable();

	return 0;
}
