static int fb_show_extra_logos(struct fb_info *info, int y, int rotate)
{
	unsigned int i;

	for (i = 0; i < fb_logo_ex_num; i++)
		y += fb_show_logo_line(info, rotate,
				       fb_logo_ex[i].logo, y, fb_logo_ex[i].n);

	return y;
}
