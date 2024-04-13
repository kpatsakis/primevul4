void hci_sock_dev_event(struct hci_dev *hdev, int event)
{
	struct hci_ev_si_device ev;

	BT_DBG("hdev %s event %d", hdev->name, event);

	/* Send event to monitor */
	if (atomic_read(&monitor_promisc)) {
		struct sk_buff *skb;

		skb = create_monitor_event(hdev, event);
		if (skb) {
			send_monitor_event(skb);
			kfree_skb(skb);
		}
	}

	/* Send event to sockets */
	ev.event  = event;
	ev.dev_id = hdev->id;
	hci_si_event(NULL, HCI_EV_SI_DEVICE, sizeof(ev), &ev);

	if (event == HCI_DEV_UNREG) {
		struct sock *sk;

		/* Detach sockets from device */
		read_lock(&hci_sk_list.lock);
		sk_for_each(sk, &hci_sk_list.head) {
			bh_lock_sock_nested(sk);
			if (hci_pi(sk)->hdev == hdev) {
				hci_pi(sk)->hdev = NULL;
				sk->sk_err = EPIPE;
				sk->sk_state = BT_OPEN;
				sk->sk_state_change(sk);

				hci_dev_put(hdev);
			}
			bh_unlock_sock(sk);
		}
		read_unlock(&hci_sk_list.lock);
	}
}
