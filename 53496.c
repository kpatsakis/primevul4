void reclaim_free(DOS_FS * fs)
{
    int reclaimed;
    uint32_t i;

    if (verbose)
	printf("Checking for unused clusters.\n");
    reclaimed = 0;
    for (i = 2; i < fs->clusters + 2; i++) {
	FAT_ENTRY curEntry;
	get_fat(&curEntry, fs->fat, i, fs);

	if (!get_owner(fs, i) && curEntry.value &&
	    !FAT_IS_BAD(fs, curEntry.value)) {
	    set_fat(fs, i, 0);
	    reclaimed++;
	}
    }
    if (reclaimed)
	printf("Reclaimed %d unused cluster%s (%llu bytes).\n", (int)reclaimed,
	       reclaimed == 1 ? "" : "s",
	       (unsigned long long)reclaimed * fs->cluster_size);
}
