static int irda_create(struct net *net, struct socket *sock, int protocol,
		       int kern)
{
	struct sock *sk;
	struct irda_sock *self;

	IRDA_DEBUG(2, "%s()\n", __func__);

	if (net != &init_net)
		return -EAFNOSUPPORT;

	/* Check for valid socket type */
	switch (sock->type) {
	case SOCK_STREAM:     /* For TTP connections with SAR disabled */
	case SOCK_SEQPACKET:  /* For TTP connections with SAR enabled */
	case SOCK_DGRAM:      /* For TTP Unitdata or LMP Ultra transfers */
		break;
	default:
		return -ESOCKTNOSUPPORT;
	}

	/* Allocate networking socket */
	sk = sk_alloc(net, PF_IRDA, GFP_KERNEL, &irda_proto);
	if (sk == NULL)
		return -ENOMEM;

	self = irda_sk(sk);
	IRDA_DEBUG(2, "%s() : self is %p\n", __func__, self);

	init_waitqueue_head(&self->query_wait);

	switch (sock->type) {
	case SOCK_STREAM:
		sock->ops = &irda_stream_ops;
		self->max_sdu_size_rx = TTP_SAR_DISABLE;
		break;
	case SOCK_SEQPACKET:
		sock->ops = &irda_seqpacket_ops;
		self->max_sdu_size_rx = TTP_SAR_UNBOUND;
		break;
	case SOCK_DGRAM:
		switch (protocol) {
#ifdef CONFIG_IRDA_ULTRA
		case IRDAPROTO_ULTRA:
			sock->ops = &irda_ultra_ops;
			/* Initialise now, because we may send on unbound
			 * sockets. Jean II */
			self->max_data_size = ULTRA_MAX_DATA - LMP_PID_HEADER;
			self->max_header_size = IRDA_MAX_HEADER + LMP_PID_HEADER;
			break;
#endif /* CONFIG_IRDA_ULTRA */
		case IRDAPROTO_UNITDATA:
			sock->ops = &irda_dgram_ops;
			/* We let Unitdata conn. be like seqpack conn. */
			self->max_sdu_size_rx = TTP_SAR_UNBOUND;
			break;
		default:
			sk_free(sk);
			return -ESOCKTNOSUPPORT;
		}
		break;
	default:
		sk_free(sk);
		return -ESOCKTNOSUPPORT;
	}

	/* Initialise networking socket struct */
	sock_init_data(sock, sk);	/* Note : set sk->sk_refcnt to 1 */
	sk->sk_family = PF_IRDA;
	sk->sk_protocol = protocol;

	/* Register as a client with IrLMP */
	self->ckey = irlmp_register_client(0, NULL, NULL, NULL);
	self->mask.word = 0xffff;
	self->rx_flow = self->tx_flow = FLOW_START;
	self->nslots = DISCOVERY_DEFAULT_SLOTS;
	self->daddr = DEV_ADDR_ANY;	/* Until we get connected */
	self->saddr = 0x0;		/* so IrLMP assign us any link */
	return 0;
}
