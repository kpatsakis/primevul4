static long media_device_setup_link(struct media_device *mdev,
				    struct media_link_desc __user *_ulink)
{
	struct media_link *link = NULL;
	struct media_link_desc ulink;
	struct media_entity *source;
	struct media_entity *sink;
	int ret;

	if (copy_from_user(&ulink, _ulink, sizeof(ulink)))
		return -EFAULT;

	/* Find the source and sink entities and link.
	 */
	source = find_entity(mdev, ulink.source.entity);
	sink = find_entity(mdev, ulink.sink.entity);

	if (source == NULL || sink == NULL)
		return -EINVAL;

	if (ulink.source.index >= source->num_pads ||
	    ulink.sink.index >= sink->num_pads)
		return -EINVAL;

	link = media_entity_find_link(&source->pads[ulink.source.index],
				      &sink->pads[ulink.sink.index]);
	if (link == NULL)
		return -EINVAL;

	/* Setup the link on both entities. */
	ret = __media_entity_setup_link(link, ulink.flags);

	if (copy_to_user(_ulink, &ulink, sizeof(ulink)))
		return -EFAULT;

	return ret;
}
