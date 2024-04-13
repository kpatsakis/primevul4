struct rds_connection *rds_conn_create(struct net *net,
				       __be32 laddr, __be32 faddr,
				       struct rds_transport *trans, gfp_t gfp)
{
	return __rds_conn_create(net, laddr, faddr, trans, gfp, 0);
}
