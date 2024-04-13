const char *fmtutil_get_windows_cb_data_type_name(unsigned int ty)
{
	const char *name = "?";

	switch(ty) {
	case 1: name="CF_TEXT"; break;
	case 2: name="CF_BITMAP"; break;
	case 3: name="CF_METAFILEPICT"; break;
	case 6: name="CF_TIFF"; break;
	case 7: name="CF_OEMTEXT"; break;
	case 8: name="CF_DIB"; break;
	case 11: name="CF_RIFF"; break;
	case 12: name="CF_WAVE"; break;
	case 13: name="CF_UNICODETEXT"; break;
	case 14: name="CF_ENHMETAFILE"; break;
	case 17: name="CF_DIBV5"; break;
	}
	return name;
}