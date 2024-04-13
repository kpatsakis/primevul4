static int hid_parser_reserved(struct hid_parser *parser, struct hid_item *item)
{
	dbg_hid("reserved item type, tag 0x%x\n", item->tag);
	return 0;
}
