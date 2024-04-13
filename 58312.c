static void feh_wm_set_bg_filled(Pixmap pmap, Imlib_Image im, int use_filelist,
		int x, int y, int w, int h)
{
	int img_w, img_h, cut_x;
	int render_w, render_h, render_x, render_y;

	if (use_filelist)
		feh_wm_load_next(&im);

	img_w = gib_imlib_image_get_width(im);
	img_h = gib_imlib_image_get_height(im);

	cut_x = (((img_w * h) > (img_h * w)) ? 1 : 0);

	render_w = (  cut_x ? ((img_h * w) / h) : img_w);
	render_h = ( !cut_x ? ((img_w * h) / w) : img_h);

	render_x = (  cut_x ? ((img_w - render_w) >> 1) : 0);
	render_y = ( !cut_x ? ((img_h - render_h) >> 1) : 0);

	gib_imlib_render_image_part_on_drawable_at_size(pmap, im,
		render_x, render_y,
		render_w, render_h,
		x, y, w, h,
		1, 0, !opt.force_aliasing);

	if (use_filelist)
		gib_imlib_free_image_and_decache(im);

	return;
}
