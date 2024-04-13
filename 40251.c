int iscsi_login_rx_data(
	struct iscsi_conn *conn,
	char *buf,
	int length)
{
	int rx_got;
	struct kvec iov;

	memset(&iov, 0, sizeof(struct kvec));
	iov.iov_len	= length;
	iov.iov_base	= buf;

	/*
	 * Initial Marker-less Interval.
	 * Add the values regardless of IFMarker/OFMarker, considering
	 * it may not be negoitated yet.
	 */
	conn->of_marker += length;

	rx_got = rx_data(conn, &iov, 1, length);
	if (rx_got != length) {
		pr_err("rx_data returned %d, expecting %d.\n",
				rx_got, length);
		return -1;
	}

	return 0 ;
}
