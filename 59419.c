lha_crc16_init(void)
{
	unsigned int i;
	static int crc16init = 0;

	if (crc16init)
		return;
	crc16init = 1;

	for (i = 0; i < 256; i++) {
		unsigned int j;
		uint16_t crc = (uint16_t)i;
		for (j = 8; j; j--)
			crc = (crc >> 1) ^ ((crc & 1) * 0xA001);
		crc16tbl[0][i] = crc;
	}

	for (i = 0; i < 256; i++) {
		crc16tbl[1][i] = (crc16tbl[0][i] >> 8)
			^ crc16tbl[0][crc16tbl[0][i] & 0xff];
	}
}
