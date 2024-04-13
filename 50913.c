int hid_open_report(struct hid_device *device)
{
	struct hid_parser *parser;
	struct hid_item item;
	unsigned int size;
	__u8 *start;
	__u8 *buf;
	__u8 *end;
	int ret;
	static int (*dispatch_type[])(struct hid_parser *parser,
				      struct hid_item *item) = {
		hid_parser_main,
		hid_parser_global,
		hid_parser_local,
		hid_parser_reserved
	};

	if (WARN_ON(device->status & HID_STAT_PARSED))
		return -EBUSY;

	start = device->dev_rdesc;
	if (WARN_ON(!start))
		return -ENODEV;
	size = device->dev_rsize;

	buf = kmemdup(start, size, GFP_KERNEL);
	if (buf == NULL)
		return -ENOMEM;

	if (device->driver->report_fixup)
		start = device->driver->report_fixup(device, buf, &size);
	else
		start = buf;

	start = kmemdup(start, size, GFP_KERNEL);
	kfree(buf);
	if (start == NULL)
		return -ENOMEM;

	device->rdesc = start;
	device->rsize = size;

	parser = vzalloc(sizeof(struct hid_parser));
	if (!parser) {
		ret = -ENOMEM;
		goto err;
	}

	parser->device = device;

	end = start + size;

	device->collection = kcalloc(HID_DEFAULT_NUM_COLLECTIONS,
				     sizeof(struct hid_collection), GFP_KERNEL);
	if (!device->collection) {
		ret = -ENOMEM;
		goto err;
	}
	device->collection_size = HID_DEFAULT_NUM_COLLECTIONS;

	ret = -EINVAL;
	while ((start = fetch_item(start, end, &item)) != NULL) {

		if (item.format != HID_ITEM_FORMAT_SHORT) {
			hid_err(device, "unexpected long global item\n");
			goto err;
		}

		if (dispatch_type[item.type](parser, &item)) {
			hid_err(device, "item %u %u %u %u parsing failed\n",
				item.format, (unsigned)item.size,
				(unsigned)item.type, (unsigned)item.tag);
			goto err;
		}

		if (start == end) {
			if (parser->collection_stack_ptr) {
				hid_err(device, "unbalanced collection at end of report description\n");
				goto err;
			}
			if (parser->local.delimiter_depth) {
				hid_err(device, "unbalanced delimiter at end of report description\n");
				goto err;
			}
			vfree(parser);
			device->status |= HID_STAT_PARSED;
			return 0;
		}
	}

	hid_err(device, "item fetching failed at offset %d\n", (int)(end - start));
err:
	vfree(parser);
	hid_close_report(device);
	return ret;
}
