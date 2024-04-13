static inline struct mount *next_slave(struct mount *p)
{
	return list_entry(p->mnt_slave.next, struct mount, mnt_slave);
}
