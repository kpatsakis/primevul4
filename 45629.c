struct sock *nfc_llcp_sock_alloc(struct socket *sock, int type, gfp_t gfp)
{
	struct sock *sk;
	struct nfc_llcp_sock *llcp_sock;

	sk = sk_alloc(&init_net, PF_NFC, gfp, &llcp_sock_proto);
	if (!sk)
		return NULL;

	llcp_sock = nfc_llcp_sock(sk);

	sock_init_data(sock, sk);
	sk->sk_state = LLCP_CLOSED;
	sk->sk_protocol = NFC_SOCKPROTO_LLCP;
	sk->sk_type = type;
	sk->sk_destruct = llcp_sock_destruct;

	llcp_sock->ssap = 0;
	llcp_sock->dsap = LLCP_SAP_SDP;
	llcp_sock->rw = LLCP_MAX_RW + 1;
	llcp_sock->miux = cpu_to_be16(LLCP_MAX_MIUX + 1);
	llcp_sock->send_n = llcp_sock->send_ack_n = 0;
	llcp_sock->recv_n = llcp_sock->recv_ack_n = 0;
	llcp_sock->remote_ready = 1;
	llcp_sock->reserved_ssap = LLCP_SAP_MAX;
	nfc_llcp_socket_remote_param_init(llcp_sock);
	skb_queue_head_init(&llcp_sock->tx_queue);
	skb_queue_head_init(&llcp_sock->tx_pending_queue);
	INIT_LIST_HEAD(&llcp_sock->accept_queue);

	if (sock != NULL)
		sock->state = SS_UNCONNECTED;

	return sk;
}
