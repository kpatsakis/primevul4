IW_IMPL(char*) iw_get_copyright_string(struct iw_context *ctx, char *dst, int dstlen)
{
	iw_translatef(ctx,0,dst,dstlen,"Copyright \xc2\xa9 %s %s",IW_COPYRIGHT_YEAR,"Jason Summers");
	return dst;
}
