int iscsi_login_tx_data(
	struct iscsi_conn *conn,
	char *pdu_buf,
	char *text_buf,
	int text_length)
{
	int length, tx_sent, iov_cnt = 1;
	struct kvec iov[2];

	length = (ISCSI_HDR_LEN + text_length);

	memset(&iov[0], 0, 2 * sizeof(struct kvec));
	iov[0].iov_len		= ISCSI_HDR_LEN;
	iov[0].iov_base		= pdu_buf;

	if (text_buf && text_length) {
		iov[1].iov_len	= text_length;
		iov[1].iov_base	= text_buf;
		iov_cnt++;
	}

	/*
	 * Initial Marker-less Interval.
	 * Add the values regardless of IFMarker/OFMarker, considering
	 * it may not be negoitated yet.
	 */
	conn->if_marker += length;

	tx_sent = tx_data(conn, &iov[0], iov_cnt, length);
	if (tx_sent != length) {
		pr_err("tx_data returned %d, expecting %d.\n",
				tx_sent, length);
		return -1;
	}

	return 0;
}
