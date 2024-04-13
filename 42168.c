static int hidp_parse(struct hid_device *hid)
{
	struct hidp_session *session = hid->driver_data;

	return hid_parse_report(session->hid, session->rd_data,
			session->rd_size);
}
