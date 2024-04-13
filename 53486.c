int getPixelInterpolated(gdImagePtr im, const double x, const double y, const int bgColor)
{
	const int xi=(int)((x) < 0 ? x - 1: x);
	const int yi=(int)((y) < 0 ? y - 1: y);
	int yii;
	int i;
	double kernel, kernel_cache_y;
	double kernel_x[12], kernel_y[4];
	double new_r = 0.0f, new_g = 0.0f, new_b = 0.0f, new_a = 0.0f;

	/* These methods use special implementations */
	if (im->interpolation_id == GD_BILINEAR_FIXED || im->interpolation_id == GD_BICUBIC_FIXED || im->interpolation_id == GD_NEAREST_NEIGHBOUR) {
		return -1;
	}

	if (im->interpolation_id == GD_WEIGHTED4) {
		return getPixelInterpolateWeight(im, x, y, bgColor);
	}

	if (im->interpolation_id == GD_NEAREST_NEIGHBOUR) {
		if (im->trueColor == 1) {
			return getPixelOverflowTC(im, xi, yi, bgColor);
		} else {
			return getPixelOverflowPalette(im, xi, yi, bgColor);
		}
	}
	if (im->interpolation) {
		for (i=0; i<4; i++) {
			kernel_x[i] = (double) im->interpolation((double)(xi+i-1-x));
			kernel_y[i] = (double) im->interpolation((double)(yi+i-1-y));
		}
	} else {
		return -1;
	}

	/*
	 * TODO: use the known fast rgba multiplication implementation once
	 * the new formats are in place
	 */
	for (yii = yi-1; yii < yi+3; yii++) {
		int xii;
		kernel_cache_y = kernel_y[yii-(yi-1)];
		if (im->trueColor) {
			for (xii=xi-1; xii<xi+3; xii++) {
				const int rgbs = getPixelOverflowTC(im, xii, yii, bgColor);

				kernel = kernel_cache_y * kernel_x[xii-(xi-1)];
				new_r += kernel * gdTrueColorGetRed(rgbs);
				new_g += kernel * gdTrueColorGetGreen(rgbs);
				new_b += kernel * gdTrueColorGetBlue(rgbs);
				new_a += kernel * gdTrueColorGetAlpha(rgbs);
			}
		} else {
			for (xii=xi-1; xii<xi+3; xii++) {
				const int rgbs = getPixelOverflowPalette(im, xii, yii, bgColor);

				kernel = kernel_cache_y * kernel_x[xii-(xi-1)];
				new_r += kernel * gdTrueColorGetRed(rgbs);
				new_g += kernel * gdTrueColorGetGreen(rgbs);
				new_b += kernel * gdTrueColorGetBlue(rgbs);
				new_a += kernel * gdTrueColorGetAlpha(rgbs);
			}
		}
	}

	new_r = CLAMP(new_r, 0, 255);
	new_g = CLAMP(new_g, 0, 255);
	new_b = CLAMP(new_b, 0, 255);
	new_a = CLAMP(new_a, 0, gdAlphaMax);

	return gdTrueColorAlpha(((int)new_r), ((int)new_g), ((int)new_b), ((int)new_a));
}
