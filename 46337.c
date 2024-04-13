mm_answer_audit_event(int socket, Buffer *m)
{
	ssh_audit_event_t event;

	debug3("%s entering", __func__);

	event = buffer_get_int(m);
	switch(event) {
	case SSH_AUTH_FAIL_PUBKEY:
	case SSH_AUTH_FAIL_HOSTBASED:
	case SSH_AUTH_FAIL_GSSAPI:
	case SSH_LOGIN_EXCEED_MAXTRIES:
	case SSH_LOGIN_ROOT_DENIED:
	case SSH_CONNECTION_CLOSE:
	case SSH_INVALID_USER:
		audit_event(event);
		break;
	default:
		fatal("Audit event type %d not permitted", event);
	}

	return (0);
}
