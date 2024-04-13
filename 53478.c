gdImageScaleBilinear(gdImagePtr im, const unsigned int new_width,
                     const unsigned int new_height)
{
	if (im->trueColor) {
		return gdImageScaleBilinearTC(im, new_width, new_height);
	} else {
		return gdImageScaleBilinearPalette(im, new_width, new_height);
	}
}
