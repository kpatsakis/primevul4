static struct hlist_head *rds_conn_bucket(__be32 laddr, __be32 faddr)
{
	static u32 rds_hash_secret __read_mostly;

	unsigned long hash;

	net_get_random_once(&rds_hash_secret, sizeof(rds_hash_secret));

	/* Pass NULL, don't need struct net for hash */
	hash = __inet_ehashfn(be32_to_cpu(laddr), 0,
			      be32_to_cpu(faddr), 0,
			      rds_hash_secret);
	return &rds_conn_hash[hash & RDS_CONNECTION_HASH_MASK];
}
