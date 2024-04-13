int oz_usb_get_desc_req(void *hpd, u8 req_id, u8 req_type, u8 desc_type,
	u8 index, __le16 windex, int offset, int len)
{
	struct oz_usb_ctx *usb_ctx = hpd;
	struct oz_pd *pd = usb_ctx->pd;
	struct oz_elt *elt;
	struct oz_get_desc_req *body;
	struct oz_elt_buf *eb = &pd->elt_buff;
	struct oz_elt_info *ei = oz_elt_info_alloc(&pd->elt_buff);

	oz_dbg(ON, "    req_type = 0x%x\n", req_type);
	oz_dbg(ON, "    desc_type = 0x%x\n", desc_type);
	oz_dbg(ON, "    index = 0x%x\n", index);
	oz_dbg(ON, "    windex = 0x%x\n", windex);
	oz_dbg(ON, "    offset = 0x%x\n", offset);
	oz_dbg(ON, "    len = 0x%x\n", len);
	if (len > 200)
		len = 200;
	if (ei == NULL)
		return -1;
	elt = (struct oz_elt *)ei->data;
	elt->length = sizeof(struct oz_get_desc_req);
	body = (struct oz_get_desc_req *)(elt+1);
	body->type = OZ_GET_DESC_REQ;
	body->req_id = req_id;
	put_unaligned(cpu_to_le16(offset), &body->offset);
	put_unaligned(cpu_to_le16(len), &body->size);
	body->req_type = req_type;
	body->desc_type = desc_type;
	body->w_index = windex;
	body->index = index;
	return oz_usb_submit_elt(eb, ei, usb_ctx, 0, 0);
}
