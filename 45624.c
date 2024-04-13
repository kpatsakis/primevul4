static int llcp_sock_getname(struct socket *sock, struct sockaddr *uaddr,
			     int *len, int peer)
{
	struct sock *sk = sock->sk;
	struct nfc_llcp_sock *llcp_sock = nfc_llcp_sock(sk);
	DECLARE_SOCKADDR(struct sockaddr_nfc_llcp *, llcp_addr, uaddr);

	if (llcp_sock == NULL || llcp_sock->dev == NULL)
		return -EBADFD;

	pr_debug("%p %d %d %d\n", sk, llcp_sock->target_idx,
		 llcp_sock->dsap, llcp_sock->ssap);

	memset(llcp_addr, 0, sizeof(*llcp_addr));
	*len = sizeof(struct sockaddr_nfc_llcp);

	llcp_addr->sa_family = AF_NFC;
	llcp_addr->dev_idx = llcp_sock->dev->idx;
	llcp_addr->target_idx = llcp_sock->target_idx;
	llcp_addr->nfc_protocol = llcp_sock->nfc_protocol;
	llcp_addr->dsap = llcp_sock->dsap;
	llcp_addr->ssap = llcp_sock->ssap;
	llcp_addr->service_name_len = llcp_sock->service_name_len;
	memcpy(llcp_addr->service_name, llcp_sock->service_name,
	       llcp_addr->service_name_len);

	return 0;
}
