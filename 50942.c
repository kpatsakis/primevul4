static int __ffs_data_do_entity(enum ffs_entity_type type,
				u8 *valuep, struct usb_descriptor_header *desc,
				void *priv)
{
	struct ffs_desc_helper *helper = priv;
	struct usb_endpoint_descriptor *d;

	ENTER();

	switch (type) {
	case FFS_DESCRIPTOR:
		break;

	case FFS_INTERFACE:
		/*
		 * Interfaces are indexed from zero so if we
		 * encountered interface "n" then there are at least
		 * "n+1" interfaces.
		 */
		if (*valuep >= helper->interfaces_count)
			helper->interfaces_count = *valuep + 1;
		break;

	case FFS_STRING:
		/*
		 * Strings are indexed from 1 (0 is magic ;) reserved
		 * for languages list or some such)
		 */
		if (*valuep > helper->ffs->strings_count)
			helper->ffs->strings_count = *valuep;
		break;

	case FFS_ENDPOINT:
		d = (void *)desc;
		helper->eps_count++;
		if (helper->eps_count >= 15)
			return -EINVAL;
		/* Check if descriptors for any speed were already parsed */
		if (!helper->ffs->eps_count && !helper->ffs->interfaces_count)
			helper->ffs->eps_addrmap[helper->eps_count] =
				d->bEndpointAddress;
		else if (helper->ffs->eps_addrmap[helper->eps_count] !=
				d->bEndpointAddress)
			return -EINVAL;
		break;
	}

	return 0;
}
