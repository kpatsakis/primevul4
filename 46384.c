mm_audit_event(ssh_audit_event_t event)
{
	Buffer m;

	debug3("%s entering", __func__);

	buffer_init(&m);
	buffer_put_int(&m, event);

	mm_request_send(pmonitor->m_recvfd, MONITOR_REQ_AUDIT_EVENT, &m);
	buffer_free(&m);
}
