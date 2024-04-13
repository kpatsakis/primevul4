void rds_conn_drop(struct rds_connection *conn)
{
	atomic_set(&conn->c_state, RDS_CONN_ERROR);
	queue_work(rds_wq, &conn->c_down_w);
}
