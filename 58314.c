static void feh_wm_set_bg_scaled(Pixmap pmap, Imlib_Image im, int use_filelist,
		int x, int y, int w, int h)
{
	if (use_filelist)
		feh_wm_load_next(&im);

	gib_imlib_render_image_on_drawable_at_size(pmap, im, x, y, w, h,
			1, 0, !opt.force_aliasing);

	if (use_filelist)
		gib_imlib_free_image_and_decache(im);

	return;
}
