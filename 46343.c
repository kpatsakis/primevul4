mm_answer_gss_checkmic(int sock, Buffer *m)
{
	gss_buffer_desc gssbuf, mic;
	OM_uint32 ret;
	u_int len;

	gssbuf.value = buffer_get_string(m, &len);
	gssbuf.length = len;
	mic.value = buffer_get_string(m, &len);
	mic.length = len;

	ret = ssh_gssapi_checkmic(gsscontext, &gssbuf, &mic);

	free(gssbuf.value);
	free(mic.value);

	buffer_clear(m);
	buffer_put_int(m, ret);

	mm_request_send(sock, MONITOR_ANS_GSSCHECKMIC, m);

	if (!GSS_ERROR(ret))
		monitor_permit(mon_dispatch, MONITOR_REQ_GSSUSEROK, 1);

	return (0);
}
