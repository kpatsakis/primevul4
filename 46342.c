mm_answer_gss_accept_ctx(int sock, Buffer *m)
{
	gss_buffer_desc in;
	gss_buffer_desc out = GSS_C_EMPTY_BUFFER;
	OM_uint32 major, minor;
	OM_uint32 flags = 0; /* GSI needs this */
	u_int len;

	in.value = buffer_get_string(m, &len);
	in.length = len;
	major = ssh_gssapi_accept_ctx(gsscontext, &in, &out, &flags);
	free(in.value);

	buffer_clear(m);
	buffer_put_int(m, major);
	buffer_put_string(m, out.value, out.length);
	buffer_put_int(m, flags);
	mm_request_send(sock, MONITOR_ANS_GSSSTEP, m);

	gss_release_buffer(&minor, &out);

	if (major == GSS_S_COMPLETE) {
		monitor_permit(mon_dispatch, MONITOR_REQ_GSSSTEP, 0);
		monitor_permit(mon_dispatch, MONITOR_REQ_GSSUSEROK, 1);
		monitor_permit(mon_dispatch, MONITOR_REQ_GSSCHECKMIC, 1);
	}
	return (0);
}
