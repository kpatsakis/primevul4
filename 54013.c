void mnt_pin_kill(struct mount *m)
{
	while (1) {
		struct hlist_node *p;
		rcu_read_lock();
		p = ACCESS_ONCE(m->mnt_pins.first);
		if (!p) {
			rcu_read_unlock();
			break;
		}
		pin_kill(hlist_entry(p, struct fs_pin, m_list));
	}
}
