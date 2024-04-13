render_pixbuf_size_prepared_cb (GdkPixbufLoader *loader,
				gint             width,
				gint             height,
				EvRenderContext *rc)
{
	int scaled_width, scaled_height;

	ev_render_context_compute_scaled_size (rc, width, height, &scaled_width, &scaled_height);
	gdk_pixbuf_loader_set_size (loader, scaled_width, scaled_height);
}
