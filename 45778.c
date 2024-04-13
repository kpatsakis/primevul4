static int vmci_transport_recv_stream_cb(void *data, struct vmci_datagram *dg)
{
	struct sock *sk;
	struct sockaddr_vm dst;
	struct sockaddr_vm src;
	struct vmci_transport_packet *pkt;
	struct vsock_sock *vsk;
	bool bh_process_pkt;
	int err;

	sk = NULL;
	err = VMCI_SUCCESS;
	bh_process_pkt = false;

	/* Ignore incoming packets from contexts without sockets, or resources
	 * that aren't vsock implementations.
	 */

	if (!vmci_transport_stream_allow(dg->src.context, -1)
	    || vmci_transport_peer_rid(dg->src.context) != dg->src.resource)
		return VMCI_ERROR_NO_ACCESS;

	if (VMCI_DG_SIZE(dg) < sizeof(*pkt))
		/* Drop datagrams that do not contain full VSock packets. */
		return VMCI_ERROR_INVALID_ARGS;

	pkt = (struct vmci_transport_packet *)dg;

	/* Find the socket that should handle this packet.  First we look for a
	 * connected socket and if there is none we look for a socket bound to
	 * the destintation address.
	 */
	vsock_addr_init(&src, pkt->dg.src.context, pkt->src_port);
	vsock_addr_init(&dst, pkt->dg.dst.context, pkt->dst_port);

	sk = vsock_find_connected_socket(&src, &dst);
	if (!sk) {
		sk = vsock_find_bound_socket(&dst);
		if (!sk) {
			/* We could not find a socket for this specified
			 * address.  If this packet is a RST, we just drop it.
			 * If it is another packet, we send a RST.  Note that
			 * we do not send a RST reply to RSTs so that we do not
			 * continually send RSTs between two endpoints.
			 *
			 * Note that since this is a reply, dst is src and src
			 * is dst.
			 */
			if (vmci_transport_send_reset_bh(&dst, &src, pkt) < 0)
				pr_err("unable to send reset\n");

			err = VMCI_ERROR_NOT_FOUND;
			goto out;
		}
	}

	/* If the received packet type is beyond all types known to this
	 * implementation, reply with an invalid message.  Hopefully this will
	 * help when implementing backwards compatibility in the future.
	 */
	if (pkt->type >= VMCI_TRANSPORT_PACKET_TYPE_MAX) {
		vmci_transport_send_invalid_bh(&dst, &src);
		err = VMCI_ERROR_INVALID_ARGS;
		goto out;
	}

	/* This handler is privileged when this module is running on the host.
	 * We will get datagram connect requests from all endpoints (even VMs
	 * that are in a restricted context). If we get one from a restricted
	 * context then the destination socket must be trusted.
	 *
	 * NOTE: We access the socket struct without holding the lock here.
	 * This is ok because the field we are interested is never modified
	 * outside of the create and destruct socket functions.
	 */
	vsk = vsock_sk(sk);
	if (!vmci_transport_allow_dgram(vsk, pkt->dg.src.context)) {
		err = VMCI_ERROR_NO_ACCESS;
		goto out;
	}

	/* We do most everything in a work queue, but let's fast path the
	 * notification of reads and writes to help data transfer performance.
	 * We can only do this if there is no process context code executing
	 * for this socket since that may change the state.
	 */
	bh_lock_sock(sk);

	if (!sock_owned_by_user(sk)) {
		/* The local context ID may be out of date, update it. */
		vsk->local_addr.svm_cid = dst.svm_cid;

		if (sk->sk_state == SS_CONNECTED)
			vmci_trans(vsk)->notify_ops->handle_notify_pkt(
					sk, pkt, true, &dst, &src,
					&bh_process_pkt);
	}

	bh_unlock_sock(sk);

	if (!bh_process_pkt) {
		struct vmci_transport_recv_pkt_info *recv_pkt_info;

		recv_pkt_info = kmalloc(sizeof(*recv_pkt_info), GFP_ATOMIC);
		if (!recv_pkt_info) {
			if (vmci_transport_send_reset_bh(&dst, &src, pkt) < 0)
				pr_err("unable to send reset\n");

			err = VMCI_ERROR_NO_MEM;
			goto out;
		}

		recv_pkt_info->sk = sk;
		memcpy(&recv_pkt_info->pkt, pkt, sizeof(recv_pkt_info->pkt));
		INIT_WORK(&recv_pkt_info->work, vmci_transport_recv_pkt_work);

		schedule_work(&recv_pkt_info->work);
		/* Clear sk so that the reference count incremented by one of
		 * the Find functions above is not decremented below.  We need
		 * that reference count for the packet handler we've scheduled
		 * to run.
		 */
		sk = NULL;
	}

out:
	if (sk)
		sock_put(sk);

	return err;
}
