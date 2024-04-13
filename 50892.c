u8 *hid_alloc_report_buf(struct hid_report *report, gfp_t flags)
{
	/*
	 * 7 extra bytes are necessary to achieve proper functionality
	 * of implement() working on 8 byte chunks
	 */

	int len = hid_report_len(report) + 7;

	return kmalloc(len, flags);
}
