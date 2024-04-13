mm_ssh_gssapi_accept_ctx(Gssctxt *ctx, gss_buffer_desc *in,
    gss_buffer_desc *out, OM_uint32 *flags)
{
	Buffer m;
	OM_uint32 major;
	u_int len;

	buffer_init(&m);
	buffer_put_string(&m, in->value, in->length);

	mm_request_send(pmonitor->m_recvfd, MONITOR_REQ_GSSSTEP, &m);
	mm_request_receive_expect(pmonitor->m_recvfd, MONITOR_ANS_GSSSTEP, &m);

	major = buffer_get_int(&m);
	out->value = buffer_get_string(&m, &len);
	out->length = len;
	if (flags)
		*flags = buffer_get_int(&m);

	buffer_free(&m);

	return (major);
}
