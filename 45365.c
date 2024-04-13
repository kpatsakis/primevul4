static inline struct pppox_sock *get_item(struct pppoe_net *pn, __be16 sid,
					unsigned char *addr, int ifindex)
{
	struct pppox_sock *po;

	read_lock_bh(&pn->hash_lock);
	po = __get_item(pn, sid, addr, ifindex);
	if (po)
		sock_hold(sk_pppox(po));
	read_unlock_bh(&pn->hash_lock);

	return po;
}
