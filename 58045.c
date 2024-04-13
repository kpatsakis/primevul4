static int cp2112_write_read_req(void *buf, u8 slave_address, u16 length,
				 u8 command, u8 *data, u8 data_length)
{
	struct cp2112_write_read_req_report *report = buf;

	if (length < 1 || length > 512
	    || data_length > sizeof(report->target_address) - 1)
		return -EINVAL;

	report->report = CP2112_DATA_WRITE_READ_REQUEST;
	report->slave_address = slave_address << 1;
	report->length = cpu_to_be16(length);
	report->target_address_length = data_length + 1;
	report->target_address[0] = command;
	memcpy(&report->target_address[1], data, data_length);
	return data_length + 6;
}
