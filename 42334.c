static int hci_sock_blacklist_add(struct hci_dev *hdev, void __user *arg)
{
	bdaddr_t bdaddr;
	int err;

	if (copy_from_user(&bdaddr, arg, sizeof(bdaddr)))
		return -EFAULT;

	hci_dev_lock(hdev);

	err = hci_blacklist_add(hdev, &bdaddr, 0);

	hci_dev_unlock(hdev);

	return err;
}
