static int cp2112_read_req(void *buf, u8 slave_address, u16 length)
{
	struct cp2112_read_req_report *report = buf;

	if (length < 1 || length > 512)
		return -EINVAL;

	report->report = CP2112_DATA_READ_REQUEST;
	report->slave_address = slave_address << 1;
	report->length = cpu_to_be16(length);
	return sizeof(*report);
}
