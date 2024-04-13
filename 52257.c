check_7zip_header_in_sfx(const char *p)
{
	switch ((unsigned char)p[5]) {
	case 0x1C:
		if (memcmp(p, _7ZIP_SIGNATURE, 6) != 0)
			return (6);
		/*
		 * Test the CRC because its extraction code has 7-Zip
		 * Magic Code, so we should do this in order not to
		 * make a mis-detection.
		 */
		if (crc32(0, (const unsigned char *)p + 12, 20)
			!= archive_le32dec(p + 8))
			return (6);
		/* Hit the header! */
		return (0);
	case 0x37: return (5);
	case 0x7A: return (4);
	case 0xBC: return (3);
	case 0xAF: return (2);
	case 0x27: return (1);
	default: return (6);
	}
}
