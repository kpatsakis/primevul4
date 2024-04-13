static int oz_usb_set_config_req(void *hpd, u8 req_id, u8 index)
{
	struct oz_usb_ctx *usb_ctx = hpd;
	struct oz_pd *pd = usb_ctx->pd;
	struct oz_elt *elt;
	struct oz_elt_buf *eb = &pd->elt_buff;
	struct oz_elt_info *ei = oz_elt_info_alloc(&pd->elt_buff);
	struct oz_set_config_req *body;

	if (ei == NULL)
		return -1;
	elt = (struct oz_elt *)ei->data;
	elt->length = sizeof(struct oz_set_config_req);
	body = (struct oz_set_config_req *)(elt+1);
	body->type = OZ_SET_CONFIG_REQ;
	body->req_id = req_id;
	body->index = index;
	return oz_usb_submit_elt(eb, ei, usb_ctx, 0, 0);
}
