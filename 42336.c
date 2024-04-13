static int hci_sock_bound_ioctl(struct sock *sk, unsigned int cmd,
				unsigned long arg)
{
	struct hci_dev *hdev = hci_pi(sk)->hdev;

	if (!hdev)
		return -EBADFD;

	switch (cmd) {
	case HCISETRAW:
		if (!capable(CAP_NET_ADMIN))
			return -EACCES;

		if (test_bit(HCI_QUIRK_RAW_DEVICE, &hdev->quirks))
			return -EPERM;

		if (arg)
			set_bit(HCI_RAW, &hdev->flags);
		else
			clear_bit(HCI_RAW, &hdev->flags);

		return 0;

	case HCIGETCONNINFO:
		return hci_get_conn_info(hdev, (void __user *) arg);

	case HCIGETAUTHINFO:
		return hci_get_auth_info(hdev, (void __user *) arg);

	case HCIBLOCKADDR:
		if (!capable(CAP_NET_ADMIN))
			return -EACCES;
		return hci_sock_blacklist_add(hdev, (void __user *) arg);

	case HCIUNBLOCKADDR:
		if (!capable(CAP_NET_ADMIN))
			return -EACCES;
		return hci_sock_blacklist_del(hdev, (void __user *) arg);

	default:
		if (hdev->ioctl)
			return hdev->ioctl(hdev, cmd, arg);
		return -EINVAL;
	}
}
