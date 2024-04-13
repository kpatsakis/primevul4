static long media_device_enum_links(struct media_device *mdev,
				    struct media_links_enum __user *ulinks)
{
	struct media_links_enum links;
	int rval;

	if (copy_from_user(&links, ulinks, sizeof(links)))
		return -EFAULT;

	rval = __media_device_enum_links(mdev, &links);
	if (rval < 0)
		return rval;

	if (copy_to_user(ulinks, &links, sizeof(*ulinks)))
		return -EFAULT;

	return 0;
}
