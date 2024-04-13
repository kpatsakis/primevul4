void hci_send_to_control(struct sk_buff *skb, struct sock *skip_sk)
{
	struct sock *sk;

	BT_DBG("len %d", skb->len);

	read_lock(&hci_sk_list.lock);

	sk_for_each(sk, &hci_sk_list.head) {
		struct sk_buff *nskb;

		/* Skip the original socket */
		if (sk == skip_sk)
			continue;

		if (sk->sk_state != BT_BOUND)
			continue;

		if (hci_pi(sk)->channel != HCI_CHANNEL_CONTROL)
			continue;

		nskb = skb_clone(skb, GFP_ATOMIC);
		if (!nskb)
			continue;

		if (sock_queue_rcv_skb(sk, nskb))
			kfree_skb(nskb);
	}

	read_unlock(&hci_sk_list.lock);
}
