const char *security_get_initial_sid_context(u32 sid)
{
	if (unlikely(sid > SECINITSID_NUM))
		return NULL;
	return initial_sid_to_string[sid];
}
