BGD_DECLARE(gdImagePtr) gdImageCreateFromGd2 (FILE * inFile)
{
	gdIOCtx *in = gdNewFileCtx (inFile);
	gdImagePtr im;

	if (in == NULL) return NULL;
	im = gdImageCreateFromGd2Ctx (in);

	in->gd_free (in);

	return im;
}
