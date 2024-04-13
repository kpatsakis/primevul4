static void ip_vs_copy_udest_compat(struct ip_vs_dest_user_kern *udest,
				   struct ip_vs_dest_user *udest_compat)
{
	memset(udest, 0, sizeof(*udest));

	udest->addr.ip		= udest_compat->addr;
	udest->port		= udest_compat->port;
	udest->conn_flags	= udest_compat->conn_flags;
	udest->weight		= udest_compat->weight;
	udest->u_threshold	= udest_compat->u_threshold;
	udest->l_threshold	= udest_compat->l_threshold;
}
