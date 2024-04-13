static void feh_wm_set_bg_maxed(Pixmap pmap, Imlib_Image im, int use_filelist,
		int x, int y, int w, int h)
{
	int img_w, img_h, border_x;
	int render_w, render_h, render_x, render_y;
	int margin_x, margin_y;

	if (use_filelist)
		feh_wm_load_next(&im);

	img_w = gib_imlib_image_get_width(im);
	img_h = gib_imlib_image_get_height(im);

	border_x = (((img_w * h) > (img_h * w)) ? 0 : 1);

	render_w = (  border_x ? ((img_w * h) / img_h) : w);
	render_h = ( !border_x ? ((img_h * w) / img_w) : h);

	if(opt.geom_flags & XValue)
		if(opt.geom_flags & XNegative)
			margin_x = (w - render_w) + opt.geom_x;
		else
			margin_x = opt.geom_x;
	else
		margin_x = (w - render_w) >> 1;

	if(opt.geom_flags & YValue)
		if(opt.geom_flags & YNegative)
			margin_y = (h - render_h) + opt.geom_y;
		else
			margin_y = opt.geom_y;
	else
		margin_y = (h - render_h) >> 1;

	render_x = x + (  border_x ? margin_x : 0);
	render_y = y + ( !border_x ? margin_y : 0);

	gib_imlib_render_image_on_drawable_at_size(pmap, im,
		render_x, render_y,
		render_w, render_h,
		1, 0, !opt.force_aliasing);

	if (use_filelist)
		gib_imlib_free_image_and_decache(im);

	return;
}
