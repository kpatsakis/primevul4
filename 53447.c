_gdScaleOneAxis(gdImagePtr pSrc, gdImagePtr dst,
				unsigned int dst_len, unsigned int row, LineContribType *contrib,
				gdAxis axis)
{
	unsigned int ndx;

	for (ndx = 0; ndx < dst_len; ndx++) {
		double r = 0, g = 0, b = 0, a = 0;
		const int left = contrib->ContribRow[ndx].Left;
		const int right = contrib->ContribRow[ndx].Right;
		int *dest = (axis == HORIZONTAL) ? 
			&dst->tpixels[row][ndx] : 
			&dst->tpixels[ndx][row];

		int i;

		/* Accumulate each channel */
		for (i = left; i <= right; i++) {
			const int left_channel = i - left;
			const int srcpx = (axis == HORIZONTAL) ?
				pSrc->tpixels[row][i] : 
				pSrc->tpixels[i][row];

			r += contrib->ContribRow[ndx].Weights[left_channel]
				* (double)(gdTrueColorGetRed(srcpx));
			g += contrib->ContribRow[ndx].Weights[left_channel]
				* (double)(gdTrueColorGetGreen(srcpx));
			b += contrib->ContribRow[ndx].Weights[left_channel]
				* (double)(gdTrueColorGetBlue(srcpx));
			a += contrib->ContribRow[ndx].Weights[left_channel]
				* (double)(gdTrueColorGetAlpha(srcpx));
		}/* for */

		*dest = gdTrueColorAlpha(uchar_clamp(r, 0xFF), uchar_clamp(g, 0xFF),
                                 uchar_clamp(b, 0xFF),
                                 uchar_clamp(a, 0x7F)); /* alpha is 0..127 */
	}/* for */
}/* _gdScaleOneAxis*/
