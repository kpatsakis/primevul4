void hci_send_to_sock(struct hci_dev *hdev, struct sk_buff *skb)
{
	struct sock *sk;
	struct sk_buff *skb_copy = NULL;

	BT_DBG("hdev %p len %d", hdev, skb->len);

	read_lock(&hci_sk_list.lock);

	sk_for_each(sk, &hci_sk_list.head) {
		struct sk_buff *nskb;

		if (sk->sk_state != BT_BOUND || hci_pi(sk)->hdev != hdev)
			continue;

		/* Don't send frame to the socket it came from */
		if (skb->sk == sk)
			continue;

		if (hci_pi(sk)->channel == HCI_CHANNEL_RAW) {
			if (is_filtered_packet(sk, skb))
				continue;
		} else if (hci_pi(sk)->channel == HCI_CHANNEL_USER) {
			if (!bt_cb(skb)->incoming)
				continue;
			if (bt_cb(skb)->pkt_type != HCI_EVENT_PKT &&
			    bt_cb(skb)->pkt_type != HCI_ACLDATA_PKT &&
			    bt_cb(skb)->pkt_type != HCI_SCODATA_PKT)
				continue;
		} else {
			/* Don't send frame to other channel types */
			continue;
		}

		if (!skb_copy) {
			/* Create a private copy with headroom */
			skb_copy = __pskb_copy(skb, 1, GFP_ATOMIC);
			if (!skb_copy)
				continue;

			/* Put type byte before the data */
			memcpy(skb_push(skb_copy, 1), &bt_cb(skb)->pkt_type, 1);
		}

		nskb = skb_clone(skb_copy, GFP_ATOMIC);
		if (!nskb)
			continue;

		if (sock_queue_rcv_skb(sk, nskb))
			kfree_skb(nskb);
	}

	read_unlock(&hci_sk_list.lock);

	kfree_skb(skb_copy);
}
