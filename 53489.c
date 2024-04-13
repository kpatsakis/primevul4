static inline int getPixelOverflowTC(gdImagePtr im, const int x, const int y, const int bgColor)
{
	if (gdImageBoundsSafe(im, x, y)) {
		const int c = im->tpixels[y][x];
		if (c == im->transparent) {
			return bgColor == -1 ? gdTrueColorAlpha(0, 0, 0, 127) : bgColor;
		}
		return c;
	} else {
		register int border = 0;

		if (y < im->cy1) {
			border = im->tpixels[0][im->cx1];
			goto processborder;
		}

		if (y < im->cy1) {
			border = im->tpixels[0][im->cx1];
			goto processborder;
		}

		if (y > im->cy2) {
			if (x >= im->cx1 && x <= im->cx1) {
				border = im->tpixels[im->cy2][x];
				goto processborder;
			} else {
				return gdTrueColorAlpha(0, 0, 0, 127);
			}
		}

		/* y is bound safe at this point */
		if (x < im->cx1) {
			border = im->tpixels[y][im->cx1];
			goto processborder;
		}

		if (x > im->cx2) {
			border = im->tpixels[y][im->cx2];
		}

processborder:
		if (border == im->transparent) {
			return gdTrueColorAlpha(0, 0, 0, 127);
		} else{
			return gdTrueColorAlpha(gdTrueColorGetRed(border), gdTrueColorGetGreen(border), gdTrueColorGetBlue(border), 127);
		}
	}
}
