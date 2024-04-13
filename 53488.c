static inline int getPixelOverflowPalette(gdImagePtr im, const int x, const int y, const int bgColor)
{
	if (gdImageBoundsSafe(im, x, y)) {
		const int c = im->pixels[y][x];
		if (c == im->transparent) {
			return bgColor == -1 ? gdTrueColorAlpha(0, 0, 0, 127) : bgColor;
		}
		return colorIndex2RGBA(c);
	} else {
		register int border = 0;
		if (y < im->cy1) {
			border = gdImageGetPixel(im, im->cx1, 0);
			goto processborder;
		}

		if (y < im->cy1) {
			border = gdImageGetPixel(im, im->cx1, 0);
			goto processborder;
		}

		if (y > im->cy2) {
			if (x >= im->cx1 && x <= im->cx1) {
				border = gdImageGetPixel(im, x,  im->cy2);
				goto processborder;
			} else {
				return gdTrueColorAlpha(0, 0, 0, 127);
			}
		}

		/* y is bound safe at this point */
		if (x < im->cx1) {
			border = gdImageGetPixel(im, im->cx1, y);
			goto processborder;
		}

		if (x > im->cx2) {
			border = gdImageGetPixel(im, im->cx2, y);
		}

processborder:
		if (border == im->transparent) {
			return gdTrueColorAlpha(0, 0, 0, 127);
		} else{
			return colorIndex2RGBcustomA(border, 127);
		}
	}
}
