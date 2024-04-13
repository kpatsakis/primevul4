static void rawsock_destruct(struct sock *sk)
{
	pr_debug("sk=%p\n", sk);

	if (sk->sk_state == TCP_ESTABLISHED) {
		nfc_deactivate_target(nfc_rawsock(sk)->dev,
				      nfc_rawsock(sk)->target_idx);
		nfc_put_device(nfc_rawsock(sk)->dev);
	}

	skb_queue_purge(&sk->sk_receive_queue);

	if (!sock_flag(sk, SOCK_DEAD)) {
		pr_err("Freeing alive NFC raw socket %p\n", sk);
		return;
	}
}
