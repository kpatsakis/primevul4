void group_pin_kill(struct hlist_head *p)
{
	while (1) {
		struct hlist_node *q;
		rcu_read_lock();
		q = ACCESS_ONCE(p->first);
		if (!q) {
			rcu_read_unlock();
			break;
		}
		pin_kill(hlist_entry(q, struct fs_pin, s_list));
	}
}
