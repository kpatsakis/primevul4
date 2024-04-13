static GFINLINE Bool isor_is_local(const char *url)
{
	if (!strnicmp(url, "file://", 7)) return GF_TRUE;
	if (!strnicmp(url, "gmem://", 7)) return GF_TRUE;
	if (!strnicmp(url, "gfio://", 7)) return GF_TRUE;
	if (!strnicmp(url, "isobmff://", 10)) return GF_TRUE;
	if (strstr(url, "://")) return GF_FALSE;
	/*the rest is local (mounted on FS)*/
	return GF_TRUE;
}