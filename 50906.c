static void hid_free_report(struct hid_report *report)
{
	unsigned n;

	for (n = 0; n < report->maxfield; n++)
		kfree(report->field[n]);
	kfree(report);
}
