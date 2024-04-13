static void send_monitor_replay(struct sock *sk)
{
	struct hci_dev *hdev;

	read_lock(&hci_dev_list_lock);

	list_for_each_entry(hdev, &hci_dev_list, list) {
		struct sk_buff *skb;

		skb = create_monitor_event(hdev, HCI_DEV_REG);
		if (!skb)
			continue;

		if (sock_queue_rcv_skb(sk, skb))
			kfree_skb(skb);
	}

	read_unlock(&hci_dev_list_lock);
}
