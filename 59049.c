static uint16_t *plist_utf8_to_utf16(char *unistr, long size, long *items_read, long *items_written)
{
	uint16_t *outbuf = (uint16_t*)malloc(((size*2)+1)*sizeof(uint16_t));
	int p = 0;
	long i = 0;

	unsigned char c0;
	unsigned char c1;
	unsigned char c2;
	unsigned char c3;

	uint32_t w;

	while (i < size) {
		c0 = unistr[i];
		c1 = (i < size-1) ? unistr[i+1] : 0;
		c2 = (i < size-2) ? unistr[i+2] : 0;
		c3 = (i < size-3) ? unistr[i+3] : 0;
		if ((c0 >= 0xF0) && (i < size-3) && (c1 >= 0x80) && (c2 >= 0x80) && (c3 >= 0x80)) {
			w = ((((c0 & 7) << 18) + ((c1 & 0x3F) << 12) + ((c2 & 0x3F) << 6) + (c3 & 0x3F)) & 0x1FFFFF) - 0x010000;
			outbuf[p++] = 0xD800 + (w >> 10);
			outbuf[p++] = 0xDC00 + (w & 0x3FF);
			i+=4;
		} else if ((c0 >= 0xE0) && (i < size-2) && (c1 >= 0x80) && (c2 >= 0x80)) {
			outbuf[p++] = ((c2 & 0x3F) + ((c1 & 3) << 6)) + (((c1 >> 2) & 15) << 8) + ((c0 & 15) << 12);
			i+=3;
		} else if ((c0 >= 0xC0) && (i < size-1) && (c1 >= 0x80)) {
			outbuf[p++] = ((c1 & 0x3F) + ((c0 & 3) << 6)) + (((c0 >> 2) & 7) << 8);
			i+=2;
		} else if (c0 < 0x80) {
			outbuf[p++] = c0;
			i+=1;
		} else {
			PLIST_BIN_ERR("%s: invalid utf8 sequence in string at index %lu\n", __func__, i);
			break;
		}
	}
	if (items_read) {
		*items_read = i;
	}
	if (items_written) {
		*items_written = p;
	}
	outbuf[p] = 0;

	return outbuf;

}
