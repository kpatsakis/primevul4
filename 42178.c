static int hidp_send_ctrl_message(struct hidp_session *session,
			unsigned char hdr, unsigned char *data, int size)
{
	int err;

	err = __hidp_send_ctrl_message(session, hdr, data, size);

	hidp_schedule(session);

	return err;
}
