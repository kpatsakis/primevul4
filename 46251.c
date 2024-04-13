static struct rds_connection *rds_conn_lookup(struct net *net,
					      struct hlist_head *head,
					      __be32 laddr, __be32 faddr,
					      struct rds_transport *trans)
{
	struct rds_connection *conn, *ret = NULL;

	hlist_for_each_entry_rcu(conn, head, c_hash_node) {
		if (conn->c_faddr == faddr && conn->c_laddr == laddr &&
		    conn->c_trans == trans && net == rds_conn_net(conn)) {
			ret = conn;
			break;
		}
	}
	rdsdebug("returning conn %p for %pI4 -> %pI4\n", ret,
		 &laddr, &faddr);
	return ret;
}
