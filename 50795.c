static inline const struct nfnetlink_subsystem *nfnetlink_get_subsys(u_int16_t type)
{
	u_int8_t subsys_id = NFNL_SUBSYS_ID(type);

	if (subsys_id >= NFNL_SUBSYS_COUNT)
		return NULL;

	return rcu_dereference(table[subsys_id].subsys);
}
