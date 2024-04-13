void hid_output_report(struct hid_report *report, __u8 *data)
{
	unsigned n;

	if (report->id > 0)
		*data++ = report->id;

	memset(data, 0, ((report->size - 1) >> 3) + 1);
	for (n = 0; n < report->maxfield; n++)
		hid_output_field(report->device, report->field[n], data);
}
