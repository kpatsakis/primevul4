static int cp2112_write_req(void *buf, u8 slave_address, u8 command, u8 *data,
			    u8 data_length)
{
	struct cp2112_write_req_report *report = buf;

	if (data_length > sizeof(report->data) - 1)
		return -EINVAL;

	report->report = CP2112_DATA_WRITE_REQUEST;
	report->slave_address = slave_address << 1;
	report->length = data_length + 1;
	report->data[0] = command;
	memcpy(&report->data[1], data, data_length);
	return data_length + 4;
}
