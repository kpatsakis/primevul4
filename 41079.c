static struct stats dx_show_leaf(struct dx_hash_info *hinfo, struct ext4_dir_entry_2 *de,
				 int size, int show_names)
{
	unsigned names = 0, space = 0;
	char *base = (char *) de;
	struct dx_hash_info h = *hinfo;

	printk("names: ");
	while ((char *) de < base + size)
	{
		if (de->inode)
		{
			if (show_names)
			{
				int len = de->name_len;
				char *name = de->name;
				while (len--) printk("%c", *name++);
				ext4fs_dirhash(de->name, de->name_len, &h);
				printk(":%x.%u ", h.hash,
				       (unsigned) ((char *) de - base));
			}
			space += EXT4_DIR_REC_LEN(de->name_len);
			names++;
		}
		de = ext4_next_entry(de, size);
	}
	printk("(%i)\n", names);
	return (struct stats) { names, space, 1 };
}
