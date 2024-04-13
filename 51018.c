BGD_DECLARE(gdImagePtr) gdImageCreateFromWebpPtr (int size, void *data)
{
	gdImagePtr im;
	gdIOCtx *in = gdNewDynamicCtxEx(size, data, 0);
	if (!in)
		return 0;
	im = gdImageCreateFromWebpCtx(in);
	in->gd_free(in);
	return im;
}
