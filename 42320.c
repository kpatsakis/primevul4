int vcc_create(struct net *net, struct socket *sock, int protocol, int family)
{
	struct sock *sk;
	struct atm_vcc *vcc;

	sock->sk = NULL;
	if (sock->type == SOCK_STREAM)
		return -EINVAL;
	sk = sk_alloc(net, family, GFP_KERNEL, &vcc_proto);
	if (!sk)
		return -ENOMEM;
	sock_init_data(sock, sk);
	sk->sk_state_change = vcc_def_wakeup;
	sk->sk_write_space = vcc_write_space;

	vcc = atm_sk(sk);
	vcc->dev = NULL;
	memset(&vcc->local, 0, sizeof(struct sockaddr_atmsvc));
	memset(&vcc->remote, 0, sizeof(struct sockaddr_atmsvc));
	vcc->qos.txtp.max_sdu = 1 << 16; /* for meta VCs */
	atomic_set(&sk->sk_wmem_alloc, 1);
	atomic_set(&sk->sk_rmem_alloc, 0);
	vcc->push = NULL;
	vcc->pop = NULL;
	vcc->push_oam = NULL;
	vcc->vpi = vcc->vci = 0; /* no VCI/VPI yet */
	vcc->atm_options = vcc->aal_options = 0;
	sk->sk_destruct = vcc_sock_destruct;
	return 0;
}
