static int cp2112_i2c_write_read_req(void *buf, u8 slave_address,
				     u8 *addr, int addr_length,
				     int read_length)
{
	struct cp2112_write_read_req_report *report = buf;

	if (read_length < 1 || read_length > 512 ||
	    addr_length > sizeof(report->target_address))
		return -EINVAL;

	report->report = CP2112_DATA_WRITE_READ_REQUEST;
	report->slave_address = slave_address << 1;
	report->length = cpu_to_be16(read_length);
	report->target_address_length = addr_length;
	memcpy(report->target_address, addr, addr_length);
	return addr_length + 5;
}
