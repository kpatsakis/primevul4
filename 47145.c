void oz_usb_farewell(struct oz_pd *pd, u8 ep_num, u8 *data, u8 len)
{
	struct oz_usb_ctx *usb_ctx;

	spin_lock_bh(&pd->app_lock[OZ_APPID_USB]);
	usb_ctx = (struct oz_usb_ctx *)pd->app_ctx[OZ_APPID_USB];
	if (usb_ctx)
		oz_usb_get(usb_ctx);
	spin_unlock_bh(&pd->app_lock[OZ_APPID_USB]);
	if (usb_ctx == NULL)
		return; /* Context has gone so nothing to do. */
	if (!usb_ctx->stopped) {
		oz_dbg(ON, "Farewell indicated ep = 0x%x\n", ep_num);
		oz_hcd_data_ind(usb_ctx->hport, ep_num, data, len);
	}
	oz_usb_put(usb_ctx);
}
