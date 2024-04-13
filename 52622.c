BGD_DECLARE(gdImagePtr) gdImageCreateFromGd2Part (FILE * inFile, int srcx, int srcy, int w, int h)
{
	gdImagePtr im;
	gdIOCtx *in = gdNewFileCtx (inFile);

	if (in == NULL) return NULL;
	im = gdImageCreateFromGd2PartCtx (in, srcx, srcy, w, h);

	in->gd_free (in);

	return im;
}
