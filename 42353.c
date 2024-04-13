static int llc_ui_create(struct net *net, struct socket *sock, int protocol,
			 int kern)
{
	struct sock *sk;
	int rc = -ESOCKTNOSUPPORT;

	if (!capable(CAP_NET_RAW))
		return -EPERM;

	if (!net_eq(net, &init_net))
		return -EAFNOSUPPORT;

	if (likely(sock->type == SOCK_DGRAM || sock->type == SOCK_STREAM)) {
		rc = -ENOMEM;
		sk = llc_sk_alloc(net, PF_LLC, GFP_KERNEL, &llc_proto);
		if (sk) {
			rc = 0;
			llc_ui_sk_init(sock, sk);
		}
	}
	return rc;
}
