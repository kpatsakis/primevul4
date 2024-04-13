BGD_DECLARE(gdImagePtr) gdImageCreateFromGd2PartPtr (int size, void *data, int srcx, int srcy, int w,
        int h)
{
	gdImagePtr im;
	gdIOCtx *in = gdNewDynamicCtxEx (size, data, 0);
	if(!in)
		return 0;
	im = gdImageCreateFromGd2PartCtx (in, srcx, srcy, w, h);
	in->gd_free (in);
	return im;
}
