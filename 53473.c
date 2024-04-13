 static void gdImageClipRectangle(gdImagePtr im, gdRectPtr r)
{
	int c1x, c1y, c2x, c2y;
	int x1,y1;

	gdImageGetClip(im, &c1x, &c1y, &c2x, &c2y);
	x1 = r->x + r->width - 1;
	y1 = r->y + r->height - 1;
	r->x = CLAMP(r->x, c1x, c2x);
	r->y = CLAMP(r->y, c1y, c2y);
	r->width = CLAMP(x1, c1x, c2x) - r->x + 1;
	r->height = CLAMP(y1, c1y, c2y) - r->y + 1;
}
