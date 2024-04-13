static int vmci_transport_recv_dgram_cb(void *data, struct vmci_datagram *dg)
{
	struct sock *sk;
	size_t size;
	struct sk_buff *skb;
	struct vsock_sock *vsk;

	sk = (struct sock *)data;

	/* This handler is privileged when this module is running on the host.
	 * We will get datagrams from all endpoints (even VMs that are in a
	 * restricted context). If we get one from a restricted context then
	 * the destination socket must be trusted.
	 *
	 * NOTE: We access the socket struct without holding the lock here.
	 * This is ok because the field we are interested is never modified
	 * outside of the create and destruct socket functions.
	 */
	vsk = vsock_sk(sk);
	if (!vmci_transport_allow_dgram(vsk, dg->src.context))
		return VMCI_ERROR_NO_ACCESS;

	size = VMCI_DG_SIZE(dg);

	/* Attach the packet to the socket's receive queue as an sk_buff. */
	skb = alloc_skb(size, GFP_ATOMIC);
	if (!skb)
		return VMCI_ERROR_NO_MEM;

	/* sk_receive_skb() will do a sock_put(), so hold here. */
	sock_hold(sk);
	skb_put(skb, size);
	memcpy(skb->data, dg, size);
	sk_receive_skb(sk, skb, 0);

	return VMCI_SUCCESS;
}
