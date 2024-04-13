parse_single_hex_dump_line(char* rec, guint8 *buf, guint byte_offset) {

	int		pos, i;
	char		*s;
	unsigned long	value;
	guint16		word_value;

	/* Get the byte_offset directly from the record */
	rec[4] = '\0';
	s = rec;
	value = strtoul(s, NULL, 16);

	if (value != byte_offset) {
		return FALSE;
	}

	/* Go through the substring representing the values and:
	 *      1. Replace any spaces with '0's
	 *      2. Place \0's every 5 bytes (to terminate the string)
	 *
	 * Then read the eight sets of hex bytes
	 */

	for (pos = START_POS; pos < START_POS + HEX_LENGTH; pos++) {
		if (rec[pos] == ' ') {
			rec[pos] = '0';
		}
	}

	pos = START_POS;
	for (i = 0; i < 8; i++) {
		rec[pos+4] = '\0';

		word_value = (guint16) strtoul(&rec[pos], NULL, 16);
		buf[byte_offset + i * 2 + 0] = (guint8) (word_value >> 8);
		buf[byte_offset + i * 2 + 1] = (guint8) (word_value & 0x00ff);
		pos += 5;
	}

	return TRUE;
}
