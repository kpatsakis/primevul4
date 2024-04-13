BGD_DECLARE(gdImagePtr) gdImageCreateFromTga(FILE *fp)
{
	gdImagePtr image;
	gdIOCtx* in = gdNewFileCtx(fp);
	if (in == NULL) return NULL;
	image = gdImageCreateFromTgaCtx(in);
	in->gd_free( in );
	return image;
}
