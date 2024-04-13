const char *fmtutil_get_windows_charset_name(u8 cs)
{
	struct csname_struct { u8 id; const char *name; };
	static const struct csname_struct csname_arr[] = {
		{0x00, "ANSI"},
		{0x01, "default"},
		{0x02, "symbol"},
		{0x4d, "Mac"},
		{0x80, "Shift-JIS"},
		{0x81, "Hangul"},
		{0x82, "Johab"},
		{0x86, "GB2312"},
		{0x88, "BIG5"},
		{0xa1, "Greek"},
		{0xa2, "Turkish"},
		{0xa3, "Vietnamese"},
		{0xb1, "Hebrew"},
		{0xb2, "Arabic"},
		{0xba, "Baltic"},
		{0xcc, "Russian"},
		{0xde, "Thai"},
		{0xee, "Eastern Europe"},
		{0xff, "OEM"}
	};
	size_t i;

	for(i=0; i<DE_ARRAYCOUNT(csname_arr); i++) {
		if(cs==csname_arr[i].id) return csname_arr[i].name;
	}
	return "?";
}