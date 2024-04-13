BGD_DECLARE(void) gdImageGd2 (gdImagePtr im, FILE * outFile, int cs, int fmt)
{
	gdIOCtx *out = gdNewFileCtx (outFile);
	if (out == NULL) return;
	_gdImageGd2 (im, out, cs, fmt);
	out->gd_free (out);
}
