parse_single_hex_dump_line(char* rec, guint8 *buf, guint byte_offset)
{
	int num_items_scanned, i;
	unsigned int bytes[16];

	num_items_scanned = sscanf(rec, "%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
			       &bytes[0], &bytes[1], &bytes[2], &bytes[3],
			       &bytes[4], &bytes[5], &bytes[6], &bytes[7],
			       &bytes[8], &bytes[9], &bytes[10], &bytes[11],
			       &bytes[12], &bytes[13], &bytes[14], &bytes[15]);
	if (num_items_scanned == 0)
		return -1;

	if (num_items_scanned > 16)
		num_items_scanned = 16;

	for (i=0; i<num_items_scanned; i++) {
		buf[byte_offset + i] = (guint8)bytes[i];
	}

	return num_items_scanned;
}
