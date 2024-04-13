static int netlink_insert(struct sock *sk, struct net *net, u32 portid)
{
	struct netlink_table *table = &nl_table[sk->sk_protocol];
	struct nl_portid_hash *hash = &table->hash;
	struct hlist_head *head;
	int err = -EADDRINUSE;
	struct sock *osk;
	int len;

	netlink_table_grab();
	head = nl_portid_hashfn(hash, portid);
	len = 0;
	sk_for_each(osk, head) {
		if (table->compare(net, osk) &&
		    (nlk_sk(osk)->portid == portid))
			break;
		len++;
	}
	if (osk)
		goto err;

	err = -EBUSY;
	if (nlk_sk(sk)->portid)
		goto err;

	err = -ENOMEM;
	if (BITS_PER_LONG > 32 && unlikely(hash->entries >= UINT_MAX))
		goto err;

	if (len && nl_portid_hash_dilute(hash, len))
		head = nl_portid_hashfn(hash, portid);
	hash->entries++;
	nlk_sk(sk)->portid = portid;
	sk_add_node(sk, head);
	err = 0;

err:
	netlink_table_ungrab();
	return err;
}
