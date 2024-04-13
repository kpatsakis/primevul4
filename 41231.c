static inline struct mount *first_slave(struct mount *p)
{
	return list_entry(p->mnt_slave_list.next, struct mount, mnt_slave);
}
