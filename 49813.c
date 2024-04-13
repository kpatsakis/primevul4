static void add_session_id(struct cryp_ctx *ctx)
{
	/*
	 * We never want 0 to be a valid value, since this is the default value
	 * for the software context.
	 */
	if (unlikely(atomic_inc_and_test(&session_id)))
		atomic_inc(&session_id);

	ctx->session_id = atomic_read(&session_id);
}
