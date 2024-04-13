void hci_send_to_monitor(struct hci_dev *hdev, struct sk_buff *skb)
{
	struct sock *sk;
	struct hlist_node *node;
	struct sk_buff *skb_copy = NULL;
	__le16 opcode;

	if (!atomic_read(&monitor_promisc))
		return;

	BT_DBG("hdev %p len %d", hdev, skb->len);

	switch (bt_cb(skb)->pkt_type) {
	case HCI_COMMAND_PKT:
		opcode = __constant_cpu_to_le16(HCI_MON_COMMAND_PKT);
		break;
	case HCI_EVENT_PKT:
		opcode = __constant_cpu_to_le16(HCI_MON_EVENT_PKT);
		break;
	case HCI_ACLDATA_PKT:
		if (bt_cb(skb)->incoming)
			opcode = __constant_cpu_to_le16(HCI_MON_ACL_RX_PKT);
		else
			opcode = __constant_cpu_to_le16(HCI_MON_ACL_TX_PKT);
		break;
	case HCI_SCODATA_PKT:
		if (bt_cb(skb)->incoming)
			opcode = __constant_cpu_to_le16(HCI_MON_SCO_RX_PKT);
		else
			opcode = __constant_cpu_to_le16(HCI_MON_SCO_TX_PKT);
		break;
	default:
		return;
	}

	read_lock(&hci_sk_list.lock);

	sk_for_each(sk, node, &hci_sk_list.head) {
		struct sk_buff *nskb;

		if (sk->sk_state != BT_BOUND)
			continue;

		if (hci_pi(sk)->channel != HCI_CHANNEL_MONITOR)
			continue;

		if (!skb_copy) {
			struct hci_mon_hdr *hdr;

			/* Create a private copy with headroom */
			skb_copy = __pskb_copy(skb, HCI_MON_HDR_SIZE,
					       GFP_ATOMIC);
			if (!skb_copy)
				continue;

			/* Put header before the data */
			hdr = (void *) skb_push(skb_copy, HCI_MON_HDR_SIZE);
			hdr->opcode = opcode;
			hdr->index = cpu_to_le16(hdev->id);
			hdr->len = cpu_to_le16(skb->len);
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
