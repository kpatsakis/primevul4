BGD_DECLARE(int) gdTransformAffineBoundingBox(gdRectPtr src, const double affine[6], gdRectPtr bbox)
{
	gdPointF extent[4], min, max, point;
	int i;

	extent[0].x=0.0;
	extent[0].y=0.0;
	extent[1].x=(double) src->width;
	extent[1].y=0.0;
	extent[2].x=(double) src->width;
	extent[2].y=(double) src->height;
	extent[3].x=0.0;
	extent[3].y=(double) src->height;

	for (i=0; i < 4; i++) {
		point=extent[i];
		if (gdAffineApplyToPointF(&extent[i], &point, affine) != GD_TRUE) {
			return GD_FALSE;
		}
	}
	min=extent[0];
	max=extent[0];

	for (i=1; i < 4; i++) {
		if (min.x > extent[i].x)
			min.x=extent[i].x;
		if (min.y > extent[i].y)
			min.y=extent[i].y;
		if (max.x < extent[i].x)
			max.x=extent[i].x;
		if (max.y < extent[i].y)
			max.y=extent[i].y;
	}
	bbox->x = (int) min.x;
	bbox->y = (int) min.y;
	bbox->width  = (int) floor(max.x - min.x) - 1;
	bbox->height = (int) floor(max.y - min.y);
	return GD_TRUE;
}
