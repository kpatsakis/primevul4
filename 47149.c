static int oz_usb_set_clear_feature_req(void *hpd, u8 req_id, u8 type,
			u8 recipient, u8 index, __le16 feature)
{
	struct oz_usb_ctx *usb_ctx = hpd;
	struct oz_pd *pd = usb_ctx->pd;
	struct oz_elt *elt;
	struct oz_elt_buf *eb = &pd->elt_buff;
	struct oz_elt_info *ei = oz_elt_info_alloc(&pd->elt_buff);
	struct oz_feature_req *body;

	if (ei == NULL)
		return -1;
	elt = (struct oz_elt *)ei->data;
	elt->length = sizeof(struct oz_feature_req);
	body = (struct oz_feature_req *)(elt+1);
	body->type = type;
	body->req_id = req_id;
	body->recipient = recipient;
	body->index = index;
	put_unaligned(feature, &body->feature);
	return oz_usb_submit_elt(eb, ei, usb_ctx, 0, 0);
}
