mm_ssh1_session_key(BIGNUM *num)
{
	int rsafail;
	Buffer m;

	buffer_init(&m);
	buffer_put_bignum2(&m, num);
	mm_request_send(pmonitor->m_recvfd, MONITOR_REQ_SESSKEY, &m);

	mm_request_receive_expect(pmonitor->m_recvfd, MONITOR_ANS_SESSKEY, &m);

	rsafail = buffer_get_int(&m);
	buffer_get_bignum2(&m, num);

	buffer_free(&m);

	return (rsafail);
}
