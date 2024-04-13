const char *fmtutil_tiff_orientation_name(i64 n)
{
	static const char *names[9] = {
		"?", "top-left", "top-right", "bottom-right", "bottom-left",
		"left-top", "right-top", "right-bottom", "left-bottom"
	};
	if(n>=1 && n<=8) return names[n];
	return names[0];
}