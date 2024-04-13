static int hidp_send_report(struct hidp_session *session, struct hid_report *report)
{
	unsigned char buf[32];
	int rsize;

	rsize = ((report->size - 1) >> 3) + 1 + (report->id > 0);
	if (rsize > sizeof(buf))
		return -EIO;

	hid_output_report(report, buf);

	return hidp_queue_report(session, buf, rsize);
}
