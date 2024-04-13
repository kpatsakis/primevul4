static inline void delete_item(struct pppoe_net *pn, __be16 sid,
					char *addr, int ifindex)
{
	write_lock_bh(&pn->hash_lock);
	__delete_item(pn, sid, addr, ifindex);
	write_unlock_bh(&pn->hash_lock);
}
