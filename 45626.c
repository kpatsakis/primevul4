static int llcp_sock_release(struct socket *sock)
{
	struct sock *sk = sock->sk;
	struct nfc_llcp_local *local;
	struct nfc_llcp_sock *llcp_sock = nfc_llcp_sock(sk);
	int err = 0;

	if (!sk)
		return 0;

	pr_debug("%p\n", sk);

	local = llcp_sock->local;
	if (local == NULL) {
		err = -ENODEV;
		goto out;
	}

	lock_sock(sk);

	/* Send a DISC */
	if (sk->sk_state == LLCP_CONNECTED)
		nfc_llcp_send_disconnect(llcp_sock);

	if (sk->sk_state == LLCP_LISTEN) {
		struct nfc_llcp_sock *lsk, *n;
		struct sock *accept_sk;

		list_for_each_entry_safe(lsk, n, &llcp_sock->accept_queue,
					 accept_queue) {
			accept_sk = &lsk->sk;
			lock_sock(accept_sk);

			nfc_llcp_send_disconnect(lsk);
			nfc_llcp_accept_unlink(accept_sk);

			release_sock(accept_sk);
		}
	}

	if (llcp_sock->reserved_ssap < LLCP_SAP_MAX)
		nfc_llcp_put_ssap(llcp_sock->local, llcp_sock->ssap);

	release_sock(sk);

	/* Keep this sock alive and therefore do not remove it from the sockets
	 * list until the DISC PDU has been actually sent. Otherwise we would
	 * reply with DM PDUs before sending the DISC one.
	 */
	if (sk->sk_state == LLCP_DISCONNECTING)
		return err;

	if (sock->type == SOCK_RAW)
		nfc_llcp_sock_unlink(&local->raw_sockets, sk);
	else
		nfc_llcp_sock_unlink(&local->sockets, sk);

out:
	sock_orphan(sk);
	sock_put(sk);

	return err;
}
