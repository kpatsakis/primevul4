static int oz_usb_submit_elt(struct oz_elt_buf *eb, struct oz_elt_info *ei,
	struct oz_usb_ctx *usb_ctx, u8 strid, u8 isoc)
{
	int ret;
	struct oz_elt *elt = (struct oz_elt *)ei->data;
	struct oz_app_hdr *app_hdr = (struct oz_app_hdr *)(elt+1);

	elt->type = OZ_ELT_APP_DATA;
	ei->app_id = OZ_APPID_USB;
	ei->length = elt->length + sizeof(struct oz_elt);
	app_hdr->app_id = OZ_APPID_USB;
	spin_lock_bh(&eb->lock);
	if (isoc == 0) {
		app_hdr->elt_seq_num = usb_ctx->tx_seq_num++;
		if (usb_ctx->tx_seq_num == 0)
			usb_ctx->tx_seq_num = 1;
	}
	ret = oz_queue_elt_info(eb, isoc, strid, ei);
	if (ret)
		oz_elt_info_free(eb, ei);
	spin_unlock_bh(&eb->lock);
	return ret;
}
