mm_answer_pam_account(int sock, Buffer *m)
{
	u_int ret;

	if (!options.use_pam)
		fatal("UsePAM not set, but ended up in %s anyway", __func__);

	ret = do_pam_account();

	buffer_put_int(m, ret);
	buffer_put_string(m, buffer_ptr(&loginmsg), buffer_len(&loginmsg));

	mm_request_send(sock, MONITOR_ANS_PAM_ACCOUNT, m);

	return (ret);
}
