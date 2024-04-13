static void read_fsinfo(DOS_FS * fs, struct boot_sector *b, int lss)
{
    struct info_sector i;

    if (!b->info_sector) {
	printf("No FSINFO sector\n");
	if (interactive)
	    printf("1) Create one\n2) Do without FSINFO\n");
	else
	    printf("  Not automatically creating it.\n");
	if (interactive && get_key("12", "?") == '1') {
	    /* search for a free reserved sector (not boot sector and not
	     * backup boot sector) */
	    uint32_t s;
	    for (s = 1; s < le16toh(b->reserved); ++s)
		if (s != le16toh(b->backup_boot))
		    break;
	    if (s > 0 && s < le16toh(b->reserved)) {
		init_fsinfo(&i);
		fs_write((off_t)s * lss, sizeof(i), &i);
		b->info_sector = htole16(s);
		fs_write(offsetof(struct boot_sector, info_sector),
			 sizeof(b->info_sector), &b->info_sector);
		if (fs->backupboot_start)
		    fs_write(fs->backupboot_start +
			     offsetof(struct boot_sector, info_sector),
			     sizeof(b->info_sector), &b->info_sector);
	    } else {
		printf("No free reserved sector found -- "
		       "no space for FSINFO sector!\n");
		return;
	    }
	} else
	    return;
    }

    fs->fsinfo_start = le16toh(b->info_sector) * lss;
    fs_read(fs->fsinfo_start, sizeof(i), &i);

    if (i.magic != htole32(0x41615252) ||
	i.signature != htole32(0x61417272) || i.boot_sign != htole16(0xaa55)) {
	printf("FSINFO sector has bad magic number(s):\n");
	if (i.magic != htole32(0x41615252))
	    printf("  Offset %llu: 0x%08x != expected 0x%08x\n",
		   (unsigned long long)offsetof(struct info_sector, magic),
		   le32toh(i.magic), 0x41615252);
	if (i.signature != htole32(0x61417272))
	    printf("  Offset %llu: 0x%08x != expected 0x%08x\n",
		   (unsigned long long)offsetof(struct info_sector, signature),
		   le32toh(i.signature), 0x61417272);
	if (i.boot_sign != htole16(0xaa55))
	    printf("  Offset %llu: 0x%04x != expected 0x%04x\n",
		   (unsigned long long)offsetof(struct info_sector, boot_sign),
		   le16toh(i.boot_sign), 0xaa55);
	if (interactive)
	    printf("1) Correct\n2) Don't correct (FSINFO invalid then)\n");
	else
	    printf("  Auto-correcting it.\n");
	if (!interactive || get_key("12", "?") == '1') {
	    init_fsinfo(&i);
	    fs_write(fs->fsinfo_start, sizeof(i), &i);
	} else
	    fs->fsinfo_start = 0;
    }

    if (fs->fsinfo_start)
	fs->free_clusters = le32toh(i.free_clusters);
}
