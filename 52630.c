_gd2CreateFromFile (gdIOCtxPtr in, int *sx, int *sy,
                    int *cs, int *vers, int *fmt,
                    int *ncx, int *ncy, t_chunk_info ** cidx)
{
	gdImagePtr im;

	if (_gd2GetHeader (in, sx, sy, cs, vers, fmt, ncx, ncy, cidx) != 1) {
		GD2_DBG (printf ("Bad GD2 header\n"));
		goto fail1;
	}
	if (gd2_truecolor (*fmt)) {
		im = gdImageCreateTrueColor (*sx, *sy);
	} else {
		im = gdImageCreate (*sx, *sy);
	}
	if (im == NULL) {
		GD2_DBG (printf ("Could not create gdImage\n"));
		goto fail2;
	};

	if (!_gdGetColors (in, im, (*vers) == 2)) {
		GD2_DBG (printf ("Could not read color palette\n"));
		goto fail3;
	}
	GD2_DBG (printf ("Image palette completed: %d colours\n", im->colorsTotal));

	return im;

fail3:
	gdImageDestroy (im);
fail2:
	gdFree(*cidx);
fail1:
	return 0;

}
