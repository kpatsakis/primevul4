BGD_DECLARE(gdImagePtr) gdImageCreateFromWebp (FILE * inFile)
{
	gdImagePtr im;
	gdIOCtx *in = gdNewFileCtx(inFile);
	if (!in) {
		return 0;
	}
	im = gdImageCreateFromWebpCtx(in);
	in->gd_free(in);

	return im;
}
