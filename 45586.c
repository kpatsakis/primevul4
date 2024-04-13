static struct sock *netlink_lookup(struct net *net, int protocol, u32 portid)
{
	struct netlink_table *table = &nl_table[protocol];
	struct nl_portid_hash *hash = &table->hash;
	struct hlist_head *head;
	struct sock *sk;

	read_lock(&nl_table_lock);
	head = nl_portid_hashfn(hash, portid);
	sk_for_each(sk, head) {
		if (table->compare(net, sk) &&
		    (nlk_sk(sk)->portid == portid)) {
			sock_hold(sk);
			goto found;
		}
	}
	sk = NULL;
found:
	read_unlock(&nl_table_lock);
	return sk;
}
