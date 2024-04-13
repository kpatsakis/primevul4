void fix_bad(DOS_FS * fs)
{
    uint32_t i;

    if (verbose)
	printf("Checking for bad clusters.\n");
    for (i = 2; i < fs->clusters + 2; i++) {
	FAT_ENTRY curEntry;
	get_fat(&curEntry, fs->fat, i, fs);

	if (!get_owner(fs, i) && !FAT_IS_BAD(fs, curEntry.value))
	    if (!fs_test(cluster_start(fs, i), fs->cluster_size)) {
		printf("Cluster %lu is unreadable.\n", (unsigned long)i);
		set_fat(fs, i, -2);
	    }
    }
}
