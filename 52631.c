_gd2PutHeader (gdImagePtr im, gdIOCtx * out, int cs, int fmt, int cx, int cy)
{
	int i;

	/* Send the gd2 id, to verify file format. */
	for (i = 0; i < 4; i++) {
		gdPutC ((unsigned char) (GD2_ID[i]), out);
	};

	/* */
	/* We put the version info first, so future versions can easily change header info. */
	/* */
	gdPutWord (GD2_VERS, out);
	gdPutWord (im->sx, out);
	gdPutWord (im->sy, out);
	gdPutWord (cs, out);
	gdPutWord (fmt, out);
	gdPutWord (cx, out);
	gdPutWord (cy, out);

}
