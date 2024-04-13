static int hci_sock_getname(struct socket *sock, struct sockaddr *addr,
			    int *addr_len, int peer)
{
	struct sockaddr_hci *haddr = (struct sockaddr_hci *) addr;
	struct sock *sk = sock->sk;
	struct hci_dev *hdev;
	int err = 0;

	BT_DBG("sock %p sk %p", sock, sk);

	if (peer)
		return -EOPNOTSUPP;

	lock_sock(sk);

	hdev = hci_pi(sk)->hdev;
	if (!hdev) {
		err = -EBADFD;
		goto done;
	}

	*addr_len = sizeof(*haddr);
	haddr->hci_family = AF_BLUETOOTH;
	haddr->hci_dev    = hdev->id;
	haddr->hci_channel= hci_pi(sk)->channel;

done:
	release_sock(sk);
	return err;
}
