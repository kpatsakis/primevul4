static int acquire_in_xmit(struct rds_connection *conn)
{
	return test_and_set_bit(RDS_IN_XMIT, &conn->c_flags) == 0;
}
