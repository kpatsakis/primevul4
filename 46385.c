mm_audit_run_command(const char *command)
{
	Buffer m;

	debug3("%s entering command %s", __func__, command);

	buffer_init(&m);
	buffer_put_cstring(&m, command);

	mm_request_send(pmonitor->m_recvfd, MONITOR_REQ_AUDIT_COMMAND, &m);
	buffer_free(&m);
}
