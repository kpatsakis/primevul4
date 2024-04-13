int sco_connect_ind(struct hci_dev *hdev, bdaddr_t *bdaddr, __u8 *flags)
{
	struct sock *sk;
	int lm = 0;

	BT_DBG("hdev %s, bdaddr %pMR", hdev->name, bdaddr);

	/* Find listening sockets */
	read_lock(&sco_sk_list.lock);
	sk_for_each(sk, &sco_sk_list.head) {
		if (sk->sk_state != BT_LISTEN)
			continue;

		if (!bacmp(&sco_pi(sk)->src, &hdev->bdaddr) ||
		    !bacmp(&sco_pi(sk)->src, BDADDR_ANY)) {
			lm |= HCI_LM_ACCEPT;

			if (test_bit(BT_SK_DEFER_SETUP, &bt_sk(sk)->flags))
				*flags |= HCI_PROTO_DEFER;
			break;
		}
	}
	read_unlock(&sco_sk_list.lock);

	return lm;
}
