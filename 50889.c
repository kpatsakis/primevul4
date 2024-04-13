static int close_collection(struct hid_parser *parser)
{
	if (!parser->collection_stack_ptr) {
		hid_err(parser->device, "collection stack underflow\n");
		return -EINVAL;
	}
	parser->collection_stack_ptr--;
	return 0;
}
