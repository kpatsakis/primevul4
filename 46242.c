void rds_conn_connect_if_down(struct rds_connection *conn)
{
	if (rds_conn_state(conn) == RDS_CONN_DOWN &&
	    !test_and_set_bit(RDS_RECONNECT_PENDING, &conn->c_flags))
		queue_delayed_work(rds_wq, &conn->c_conn_w, 0);
}
