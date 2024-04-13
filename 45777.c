static void vmci_transport_recv_pkt_work(struct work_struct *work)
{
	struct vmci_transport_recv_pkt_info *recv_pkt_info;
	struct vmci_transport_packet *pkt;
	struct sock *sk;

	recv_pkt_info =
		container_of(work, struct vmci_transport_recv_pkt_info, work);
	sk = recv_pkt_info->sk;
	pkt = &recv_pkt_info->pkt;

	lock_sock(sk);

	/* The local context ID may be out of date. */
	vsock_sk(sk)->local_addr.svm_cid = pkt->dg.dst.context;

	switch (sk->sk_state) {
	case SS_LISTEN:
		vmci_transport_recv_listen(sk, pkt);
		break;
	case SS_CONNECTING:
		/* Processing of pending connections for servers goes through
		 * the listening socket, so see vmci_transport_recv_listen()
		 * for that path.
		 */
		vmci_transport_recv_connecting_client(sk, pkt);
		break;
	case SS_CONNECTED:
		vmci_transport_recv_connected(sk, pkt);
		break;
	default:
		/* Because this function does not run in the same context as
		 * vmci_transport_recv_stream_cb it is possible that the
		 * socket has closed. We need to let the other side know or it
		 * could be sitting in a connect and hang forever. Send a
		 * reset to prevent that.
		 */
		vmci_transport_send_reset(sk, pkt);
		break;
	}

	release_sock(sk);
	kfree(recv_pkt_info);
	/* Release reference obtained in the stream callback when we fetched
	 * this socket out of the bound or connected list.
	 */
	sock_put(sk);
}
