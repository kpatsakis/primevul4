static int rawsock_connect(struct socket *sock, struct sockaddr *_addr,
			   int len, int flags)
{
	struct sock *sk = sock->sk;
	struct sockaddr_nfc *addr = (struct sockaddr_nfc *)_addr;
	struct nfc_dev *dev;
	int rc = 0;

	pr_debug("sock=%p sk=%p flags=%d\n", sock, sk, flags);

	if (!addr || len < sizeof(struct sockaddr_nfc) ||
	    addr->sa_family != AF_NFC)
		return -EINVAL;

	pr_debug("addr dev_idx=%u target_idx=%u protocol=%u\n",
		 addr->dev_idx, addr->target_idx, addr->nfc_protocol);

	lock_sock(sk);

	if (sock->state == SS_CONNECTED) {
		rc = -EISCONN;
		goto error;
	}

	dev = nfc_get_device(addr->dev_idx);
	if (!dev) {
		rc = -ENODEV;
		goto error;
	}

	if (addr->target_idx > dev->target_next_idx - 1 ||
	    addr->target_idx < dev->target_next_idx - dev->n_targets) {
		rc = -EINVAL;
		goto error;
	}

	rc = nfc_activate_target(dev, addr->target_idx, addr->nfc_protocol);
	if (rc)
		goto put_dev;

	nfc_rawsock(sk)->dev = dev;
	nfc_rawsock(sk)->target_idx = addr->target_idx;
	sock->state = SS_CONNECTED;
	sk->sk_state = TCP_ESTABLISHED;
	sk->sk_state_change(sk);

	release_sock(sk);
	return 0;

put_dev:
	nfc_put_device(dev);
error:
	release_sock(sk);
	return rc;
}
