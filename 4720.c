void fmtutil_get_bmp_compression_name(u32 code, char *s, size_t s_len,
	int is_os2v2)
{
	const char *name1 = "?";
	switch(code) {
	case 0: name1 = "BI_RGB, uncompressed"; break;
	case 1: name1 = "BI_RLE8"; break;
	case 2: name1 = "BI_RLE4"; break;
	case 3:
		if(is_os2v2)
			name1 = "Huffman 1D";
		else
			name1 = "BI_BITFIELDS, uncompressed";
		break;
	case 4:
		if(is_os2v2)
			name1 = "RLE24";
		else
			name1 = "BI_JPEG";
		break;
	case 5: name1 = "BI_PNG"; break;
	}
	de_strlcpy(s, name1, s_len);
}