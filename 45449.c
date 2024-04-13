static int hci_sock_release(struct socket *sock)
{
	struct sock *sk = sock->sk;
	struct hci_dev *hdev;

	BT_DBG("sock %p sk %p", sock, sk);

	if (!sk)
		return 0;

	hdev = hci_pi(sk)->hdev;

	if (hci_pi(sk)->channel == HCI_CHANNEL_MONITOR)
		atomic_dec(&monitor_promisc);

	bt_sock_unlink(&hci_sk_list, sk);

	if (hdev) {
		if (hci_pi(sk)->channel == HCI_CHANNEL_USER) {
			mgmt_index_added(hdev);
			clear_bit(HCI_USER_CHANNEL, &hdev->dev_flags);
			hci_dev_close(hdev->id);
		}

		atomic_dec(&hdev->promisc);
		hci_dev_put(hdev);
	}

	sock_orphan(sk);

	skb_queue_purge(&sk->sk_receive_queue);
	skb_queue_purge(&sk->sk_write_queue);

	sock_put(sk);
	return 0;
}
