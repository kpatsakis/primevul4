void read_fat(DOS_FS * fs)
{
    int eff_size, alloc_size;
    uint32_t i;
    void *first, *second = NULL;
    int first_ok, second_ok;
    uint32_t total_num_clusters;

    /* Clean up from previous pass */
    if (fs->fat)
	free(fs->fat);
    if (fs->cluster_owner)
	free(fs->cluster_owner);
    fs->fat = NULL;
    fs->cluster_owner = NULL;

    total_num_clusters = fs->clusters + 2UL;
    eff_size = (total_num_clusters * fs->fat_bits + 7) / 8ULL;

    if (fs->fat_bits != 12)
	    alloc_size = eff_size;
    else
	    /* round up to an even number of FAT entries to avoid special
	     * casing the last entry in get_fat() */
	    alloc_size = (total_num_clusters * 12 + 23) / 24 * 3;

    first = alloc(alloc_size);
    fs_read(fs->fat_start, eff_size, first);
    if (fs->nfats > 1) {
	second = alloc(alloc_size);
	fs_read(fs->fat_start + fs->fat_size, eff_size, second);
    }
    if (second && memcmp(first, second, eff_size) != 0) {
	FAT_ENTRY first_media, second_media;
	get_fat(&first_media, first, 0, fs);
	get_fat(&second_media, second, 0, fs);
	first_ok = (first_media.value & FAT_EXTD(fs)) == FAT_EXTD(fs);
	second_ok = (second_media.value & FAT_EXTD(fs)) == FAT_EXTD(fs);
	if (first_ok && !second_ok) {
	    printf("FATs differ - using first FAT.\n");
	    fs_write(fs->fat_start + fs->fat_size, eff_size, first);
	}
	if (!first_ok && second_ok) {
	    printf("FATs differ - using second FAT.\n");
	    fs_write(fs->fat_start, eff_size, second);
	    memcpy(first, second, eff_size);
	}
	if (first_ok && second_ok) {
	    if (interactive) {
		printf("FATs differ but appear to be intact. Use which FAT ?\n"
		       "1) Use first FAT\n2) Use second FAT\n");
		if (get_key("12", "?") == '1') {
		    fs_write(fs->fat_start + fs->fat_size, eff_size, first);
		} else {
		    fs_write(fs->fat_start, eff_size, second);
		    memcpy(first, second, eff_size);
		}
	    } else {
		printf("FATs differ but appear to be intact. Using first "
		       "FAT.\n");
		fs_write(fs->fat_start + fs->fat_size, eff_size, first);
	    }
	}
	if (!first_ok && !second_ok) {
	    printf("Both FATs appear to be corrupt. Giving up.\n");
	    exit(1);
	}
    }
    if (second) {
	free(second);
    }
    fs->fat = (unsigned char *)first;

    fs->cluster_owner = alloc(total_num_clusters * sizeof(DOS_FILE *));
    memset(fs->cluster_owner, 0, (total_num_clusters * sizeof(DOS_FILE *)));

    /* Truncate any cluster chains that link to something out of range */
    for (i = 2; i < fs->clusters + 2; i++) {
	FAT_ENTRY curEntry;
	get_fat(&curEntry, fs->fat, i, fs);
	if (curEntry.value == 1) {
	    printf("Cluster %ld out of range (1). Setting to EOF.\n", (long)(i - 2));
	    set_fat(fs, i, -1);
	}
	if (curEntry.value >= fs->clusters + 2 &&
	    (curEntry.value < FAT_MIN_BAD(fs))) {
	    printf("Cluster %ld out of range (%ld > %ld). Setting to EOF.\n",
		   (long)(i - 2), (long)curEntry.value, (long)(fs->clusters + 2 - 1));
	    set_fat(fs, i, -1);
	}
    }
}
