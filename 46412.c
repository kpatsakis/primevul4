mm_ssh_gssapi_checkmic(Gssctxt *ctx, gss_buffer_t gssbuf, gss_buffer_t gssmic)
{
	Buffer m;
	OM_uint32 major;

	buffer_init(&m);
	buffer_put_string(&m, gssbuf->value, gssbuf->length);
	buffer_put_string(&m, gssmic->value, gssmic->length);

	mm_request_send(pmonitor->m_recvfd, MONITOR_REQ_GSSCHECKMIC, &m);
	mm_request_receive_expect(pmonitor->m_recvfd, MONITOR_ANS_GSSCHECKMIC,
	    &m);

	major = buffer_get_int(&m);
	buffer_free(&m);
	return(major);
}
