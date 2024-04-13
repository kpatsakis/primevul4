static void check_backup_boot(DOS_FS * fs, struct boot_sector *b, int lss)
{
    struct boot_sector b2;

    if (!fs->backupboot_start) {
	printf("There is no backup boot sector.\n");
	if (le16toh(b->reserved) < 3) {
	    printf("And there is no space for creating one!\n");
	    return;
	}
	if (interactive)
	    printf("1) Create one\n2) Do without a backup\n");
	else
	    printf("  Auto-creating backup boot block.\n");
	if (!interactive || get_key("12", "?") == '1') {
	    int bbs;
	    /* The usual place for the backup boot sector is sector 6. Choose
	     * that or the last reserved sector. */
	    if (le16toh(b->reserved) >= 7 && le16toh(b->info_sector) != 6)
		bbs = 6;
	    else {
		bbs = le16toh(b->reserved) - 1;
		if (bbs == le16toh(b->info_sector))
		    --bbs;	/* this is never 0, as we checked reserved >= 3! */
	    }
	    fs->backupboot_start = bbs * lss;
	    b->backup_boot = htole16(bbs);
	    fs_write(fs->backupboot_start, sizeof(*b), b);
	    fs_write(offsetof(struct boot_sector, backup_boot),
		     sizeof(b->backup_boot), &b->backup_boot);
	    printf("Created backup of boot sector in sector %d\n", bbs);
	    return;
	} else
	    return;
    }

    fs_read(fs->backupboot_start, sizeof(b2), &b2);
    if (memcmp(b, &b2, sizeof(b2)) != 0) {
	/* there are any differences */
	uint8_t *p, *q;
	int i, pos, first = 1;
	char buf[20];

	printf("There are differences between boot sector and its backup.\n");
	printf("This is mostly harmless. Differences: (offset:original/backup)\n  ");
	pos = 2;
	for (p = (uint8_t *) b, q = (uint8_t *) & b2, i = 0; i < sizeof(b2);
	     ++p, ++q, ++i) {
	    if (*p != *q) {
		sprintf(buf, "%s%u:%02x/%02x", first ? "" : ", ",
			(unsigned)(p - (uint8_t *) b), *p, *q);
		if (pos + strlen(buf) > 78)
		    printf("\n  "), pos = 2;
		printf("%s", buf);
		pos += strlen(buf);
		first = 0;
	    }
	}
	printf("\n");

	if (interactive)
	    printf("1) Copy original to backup\n"
		   "2) Copy backup to original\n" "3) No action\n");
	else
	    printf("  Not automatically fixing this.\n");
	switch (interactive ? get_key("123", "?") : '3') {
	case '1':
	    fs_write(fs->backupboot_start, sizeof(*b), b);
	    break;
	case '2':
	    fs_write(0, sizeof(b2), &b2);
	    break;
	default:
	    break;
	}
    }
}
