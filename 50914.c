static void hid_output_field(const struct hid_device *hid,
			     struct hid_field *field, __u8 *data)
{
	unsigned count = field->report_count;
	unsigned offset = field->report_offset;
	unsigned size = field->report_size;
	unsigned n;

	for (n = 0; n < count; n++) {
		if (field->logical_minimum < 0)	/* signed values */
			implement(hid, data, offset + n * size, size,
				  s32ton(field->value[n], size));
		else				/* unsigned values */
			implement(hid, data, offset + n * size, size,
				  field->value[n]);
	}
}
