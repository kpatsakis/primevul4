nfnetlink_find_client(u_int16_t type, const struct nfnetlink_subsystem *ss)
{
	u_int8_t cb_id = NFNL_MSG_TYPE(type);

	if (cb_id >= ss->cb_count)
		return NULL;

	return &ss->cb[cb_id];
}
