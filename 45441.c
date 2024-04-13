static int hci_sock_blacklist_del(struct hci_dev *hdev, void __user *arg)
{
	bdaddr_t bdaddr;
	int err;

	if (copy_from_user(&bdaddr, arg, sizeof(bdaddr)))
		return -EFAULT;

	hci_dev_lock(hdev);

	err = hci_blacklist_del(hdev, &bdaddr, BDADDR_BREDR);

	hci_dev_unlock(hdev);

	return err;
}
