static inline int _color_blend (const int dst, const int src)
{
	const int src_alpha = gdTrueColorGetAlpha(src);

	if( src_alpha == gdAlphaOpaque ) {
		return src;
	} else {
		const int dst_alpha = gdTrueColorGetAlpha(dst);

		if( src_alpha == gdAlphaTransparent ) return dst;
		if( dst_alpha == gdAlphaTransparent ) {
			return src;
		} else {
			register int alpha, red, green, blue;
			const int src_weight = gdAlphaTransparent - src_alpha;
			const int dst_weight = (gdAlphaTransparent - dst_alpha) * src_alpha / gdAlphaMax;
			const int tot_weight = src_weight + dst_weight;

			alpha = src_alpha * dst_alpha / gdAlphaMax;

			red = (gdTrueColorGetRed(src) * src_weight
				   + gdTrueColorGetRed(dst) * dst_weight) / tot_weight;
			green = (gdTrueColorGetGreen(src) * src_weight
				   + gdTrueColorGetGreen(dst) * dst_weight) / tot_weight;
			blue = (gdTrueColorGetBlue(src) * src_weight
				   + gdTrueColorGetBlue(dst) * dst_weight) / tot_weight;

			return ((alpha << 24) + (red << 16) + (green << 8) + blue);
		}
	}
}
