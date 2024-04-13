static int sco_connect(struct sock *sk)
{
	struct sco_conn *conn;
	struct hci_conn *hcon;
	struct hci_dev  *hdev;
	int err, type;

	BT_DBG("%pMR -> %pMR", &sco_pi(sk)->src, &sco_pi(sk)->dst);

	hdev = hci_get_route(&sco_pi(sk)->dst, &sco_pi(sk)->src);
	if (!hdev)
		return -EHOSTUNREACH;

	hci_dev_lock(hdev);

	if (lmp_esco_capable(hdev) && !disable_esco)
		type = ESCO_LINK;
	else
		type = SCO_LINK;

	if (sco_pi(sk)->setting == BT_VOICE_TRANSPARENT &&
	    (!lmp_transp_capable(hdev) || !lmp_esco_capable(hdev))) {
		err = -EOPNOTSUPP;
		goto done;
	}

	hcon = hci_connect_sco(hdev, type, &sco_pi(sk)->dst,
			       sco_pi(sk)->setting);
	if (IS_ERR(hcon)) {
		err = PTR_ERR(hcon);
		goto done;
	}

	conn = sco_conn_add(hcon);
	if (!conn) {
		hci_conn_drop(hcon);
		err = -ENOMEM;
		goto done;
	}

	/* Update source addr of the socket */
	bacpy(&sco_pi(sk)->src, &hcon->src);

	err = sco_chan_add(conn, sk, NULL);
	if (err)
		goto done;

	if (hcon->state == BT_CONNECTED) {
		sco_sock_clear_timer(sk);
		sk->sk_state = BT_CONNECTED;
	} else {
		sk->sk_state = BT_CONNECT;
		sco_sock_set_timer(sk, sk->sk_sndtimeo);
	}

done:
	hci_dev_unlock(hdev);
	hci_dev_put(hdev);
	return err;
}
