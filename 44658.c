static void media_device_kpad_to_upad(const struct media_pad *kpad,
				      struct media_pad_desc *upad)
{
	upad->entity = kpad->entity->id;
	upad->index = kpad->index;
	upad->flags = kpad->flags;
}
