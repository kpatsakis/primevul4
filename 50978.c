static int ffs_ep_addr2idx(struct ffs_data *ffs, u8 endpoint_address)
{
	int i;

	for (i = 1; i < ARRAY_SIZE(ffs->eps_addrmap); ++i)
		if (ffs->eps_addrmap[i] == endpoint_address)
			return i;
	return -ENOENT;
}
