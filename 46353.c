mm_answer_pam_start(int sock, Buffer *m)
{
	if (!options.use_pam)
		fatal("UsePAM not set, but ended up in %s anyway", __func__);

	start_pam(authctxt);

	monitor_permit(mon_dispatch, MONITOR_REQ_PAM_ACCOUNT, 1);

	return (0);
}
