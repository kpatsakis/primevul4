int security_sid_to_context(u32 sid, char **scontext, u32 *scontext_len)
{
	return security_sid_to_context_core(sid, scontext, scontext_len, 0);
}
