int security_context_to_sid(const char *scontext, u32 scontext_len, u32 *sid)
{
	return security_context_to_sid_core(scontext, scontext_len,
					    sid, SECSID_NULL, GFP_KERNEL, 0);
}
