BGD_DECLARE(int) gdImageSetInterpolationMethod(gdImagePtr im, gdInterpolationMethod id)
{
	if (im == NULL || id < 0 || id > GD_METHOD_COUNT) {
		return 0;
	}

	switch (id) {
		case GD_DEFAULT:
			id = GD_BILINEAR_FIXED;
		/* Optimized versions */
		case GD_BILINEAR_FIXED:
		case GD_BICUBIC_FIXED:
		case GD_NEAREST_NEIGHBOUR:
		case GD_WEIGHTED4:
			im->interpolation = NULL;
			break;

		/* generic versions*/
		case GD_BELL:
			im->interpolation = filter_bell;
			break;
		case GD_BESSEL:
			im->interpolation = filter_bessel;
			break;
		case GD_BICUBIC:
			im->interpolation = filter_bicubic;
			break;
		case GD_BLACKMAN:
			im->interpolation = filter_blackman;
			break;
		case GD_BOX:
			im->interpolation = filter_box;
			break;
		case GD_BSPLINE:
			im->interpolation = filter_bspline;
			break;
		case GD_CATMULLROM:
			im->interpolation = filter_catmullrom;
			break;
		case GD_GAUSSIAN:
			im->interpolation = filter_gaussian;
			break;
		case GD_GENERALIZED_CUBIC:
			im->interpolation = filter_generalized_cubic;
			break;
		case GD_HERMITE:
			im->interpolation = filter_hermite;
			break;
		case GD_HAMMING:
			im->interpolation = filter_hamming;
			break;
		case GD_HANNING:
			im->interpolation = filter_hanning;
			break;
		case GD_MITCHELL:
			im->interpolation = filter_mitchell;
			break;
		case GD_POWER:
			im->interpolation = filter_power;
			break;
		case GD_QUADRATIC:
			im->interpolation = filter_quadratic;
			break;
		case GD_SINC:
			im->interpolation = filter_sinc;
			break;
		case GD_TRIANGLE:
			im->interpolation = filter_triangle;
			break;

		default:
			return 0;
			break;
	}
	im->interpolation_id = id;
	return 1;
}
