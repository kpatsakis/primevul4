static int ax25_create(struct net *net, struct socket *sock, int protocol,
		       int kern)
{
	struct sock *sk;
	ax25_cb *ax25;

	if (!net_eq(net, &init_net))
		return -EAFNOSUPPORT;

	switch (sock->type) {
	case SOCK_DGRAM:
		if (protocol == 0 || protocol == PF_AX25)
			protocol = AX25_P_TEXT;
		break;

	case SOCK_SEQPACKET:
		switch (protocol) {
		case 0:
		case PF_AX25:	/* For CLX */
			protocol = AX25_P_TEXT;
			break;
		case AX25_P_SEGMENT:
#ifdef CONFIG_INET
		case AX25_P_ARP:
		case AX25_P_IP:
#endif
#ifdef CONFIG_NETROM
		case AX25_P_NETROM:
#endif
#ifdef CONFIG_ROSE
		case AX25_P_ROSE:
#endif
			return -ESOCKTNOSUPPORT;
#ifdef CONFIG_NETROM_MODULE
		case AX25_P_NETROM:
			if (ax25_protocol_is_registered(AX25_P_NETROM))
				return -ESOCKTNOSUPPORT;
			break;
#endif
#ifdef CONFIG_ROSE_MODULE
		case AX25_P_ROSE:
			if (ax25_protocol_is_registered(AX25_P_ROSE))
				return -ESOCKTNOSUPPORT;
#endif
		default:
			break;
		}
		break;

	case SOCK_RAW:
		break;
	default:
		return -ESOCKTNOSUPPORT;
	}

	sk = sk_alloc(net, PF_AX25, GFP_ATOMIC, &ax25_proto);
	if (sk == NULL)
		return -ENOMEM;

	ax25 = sk->sk_protinfo = ax25_create_cb();
	if (!ax25) {
		sk_free(sk);
		return -ENOMEM;
	}

	sock_init_data(sock, sk);

	sk->sk_destruct = ax25_free_sock;
	sock->ops    = &ax25_proto_ops;
	sk->sk_protocol = protocol;

	ax25->sk    = sk;

	return 0;
}
