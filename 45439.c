static int hci_sock_bind(struct socket *sock, struct sockaddr *addr,
			 int addr_len)
{
	struct sockaddr_hci haddr;
	struct sock *sk = sock->sk;
	struct hci_dev *hdev = NULL;
	int len, err = 0;

	BT_DBG("sock %p sk %p", sock, sk);

	if (!addr)
		return -EINVAL;

	memset(&haddr, 0, sizeof(haddr));
	len = min_t(unsigned int, sizeof(haddr), addr_len);
	memcpy(&haddr, addr, len);

	if (haddr.hci_family != AF_BLUETOOTH)
		return -EINVAL;

	lock_sock(sk);

	if (sk->sk_state == BT_BOUND) {
		err = -EALREADY;
		goto done;
	}

	switch (haddr.hci_channel) {
	case HCI_CHANNEL_RAW:
		if (hci_pi(sk)->hdev) {
			err = -EALREADY;
			goto done;
		}

		if (haddr.hci_dev != HCI_DEV_NONE) {
			hdev = hci_dev_get(haddr.hci_dev);
			if (!hdev) {
				err = -ENODEV;
				goto done;
			}

			atomic_inc(&hdev->promisc);
		}

		hci_pi(sk)->hdev = hdev;
		break;

	case HCI_CHANNEL_USER:
		if (hci_pi(sk)->hdev) {
			err = -EALREADY;
			goto done;
		}

		if (haddr.hci_dev == HCI_DEV_NONE) {
			err = -EINVAL;
			goto done;
		}

		if (!capable(CAP_NET_ADMIN)) {
			err = -EPERM;
			goto done;
		}

		hdev = hci_dev_get(haddr.hci_dev);
		if (!hdev) {
			err = -ENODEV;
			goto done;
		}

		if (test_bit(HCI_UP, &hdev->flags) ||
		    test_bit(HCI_INIT, &hdev->flags) ||
		    test_bit(HCI_SETUP, &hdev->dev_flags)) {
			err = -EBUSY;
			hci_dev_put(hdev);
			goto done;
		}

		if (test_and_set_bit(HCI_USER_CHANNEL, &hdev->dev_flags)) {
			err = -EUSERS;
			hci_dev_put(hdev);
			goto done;
		}

		mgmt_index_removed(hdev);

		err = hci_dev_open(hdev->id);
		if (err) {
			clear_bit(HCI_USER_CHANNEL, &hdev->dev_flags);
			hci_dev_put(hdev);
			goto done;
		}

		atomic_inc(&hdev->promisc);

		hci_pi(sk)->hdev = hdev;
		break;

	case HCI_CHANNEL_CONTROL:
		if (haddr.hci_dev != HCI_DEV_NONE) {
			err = -EINVAL;
			goto done;
		}

		if (!capable(CAP_NET_ADMIN)) {
			err = -EPERM;
			goto done;
		}

		break;

	case HCI_CHANNEL_MONITOR:
		if (haddr.hci_dev != HCI_DEV_NONE) {
			err = -EINVAL;
			goto done;
		}

		if (!capable(CAP_NET_RAW)) {
			err = -EPERM;
			goto done;
		}

		send_monitor_replay(sk);

		atomic_inc(&monitor_promisc);
		break;

	default:
		err = -EINVAL;
		goto done;
	}


	hci_pi(sk)->channel = haddr.hci_channel;
	sk->sk_state = BT_BOUND;

done:
	release_sock(sk);
	return err;
}
