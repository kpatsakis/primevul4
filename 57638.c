IW_IMPL(char*) iw_get_version_string(struct iw_context *ctx, char *s, int s_len)
{
	int ver;
	ver = iw_get_version_int();
	iw_snprintf(s,s_len,"%d.%d.%d",
		(ver&0xff0000)>>16, (ver&0xff00)>>8, (ver&0xff) );
	return s;
}
