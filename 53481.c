gdImageScaleNearestNeighbour(gdImagePtr im, const unsigned int width, const unsigned int height)
{
	const unsigned long new_width = MAX(1, width);
	const unsigned long new_height = MAX(1, height);
	const float dx = (float)im->sx / (float)new_width;
	const float dy = (float)im->sy / (float)new_height;
	const gdFixed f_dx = gd_ftofx(dx);
	const gdFixed f_dy = gd_ftofx(dy);

	gdImagePtr dst_img;
	unsigned long  dst_offset_x;
	unsigned long  dst_offset_y = 0;
	unsigned int i;

	dst_img = gdImageCreateTrueColor(new_width, new_height);

	if (dst_img == NULL) {
		return NULL;
	}

	for (i=0; i<new_height; i++) {
		unsigned int j;
		dst_offset_x = 0;
		if (im->trueColor) {
			for (j=0; j<new_width; j++) {
				const gdFixed f_i = gd_itofx(i);
				const gdFixed f_j = gd_itofx(j);
				const gdFixed f_a = gd_mulfx(f_i, f_dy);
				const gdFixed f_b = gd_mulfx(f_j, f_dx);
				const long m = gd_fxtoi(f_a);
				const long n = gd_fxtoi(f_b);

				dst_img->tpixels[dst_offset_y][dst_offset_x++] = im->tpixels[m][n];
			}
		} else {
			for (j=0; j<new_width; j++) {
				const gdFixed f_i = gd_itofx(i);
				const gdFixed f_j = gd_itofx(j);
				const gdFixed f_a = gd_mulfx(f_i, f_dy);
				const gdFixed f_b = gd_mulfx(f_j, f_dx);
				const long m = gd_fxtoi(f_a);
				const long n = gd_fxtoi(f_b);

				dst_img->tpixels[dst_offset_y][dst_offset_x++] = colorIndex2RGBA(im->pixels[m][n]);
			}
		}
		dst_offset_y++;
	}
	return dst_img;
}
