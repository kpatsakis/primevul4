off_t find_volume_de(DOS_FS * fs, DIR_ENT * de)
{
    uint32_t cluster;
    off_t offset;
    int i;

    if (fs->root_cluster) {
	for (cluster = fs->root_cluster;
	     cluster != 0 && cluster != -1;
	     cluster = next_cluster(fs, cluster)) {
	    offset = cluster_start(fs, cluster);
	    for (i = 0; i * sizeof(DIR_ENT) < fs->cluster_size; i++) {
		fs_read(offset, sizeof(DIR_ENT), de);
		if (de->attr != VFAT_LN_ATTR && de->attr & ATTR_VOLUME)
		    return offset;
		offset += sizeof(DIR_ENT);
	    }
	}
    } else {
	for (i = 0; i < fs->root_entries; i++) {
	    offset = fs->root_start + i * sizeof(DIR_ENT);
	    fs_read(offset, sizeof(DIR_ENT), de);
	    if (de->attr != VFAT_LN_ATTR && de->attr & ATTR_VOLUME)
		return offset;
	}
    }

    return 0;
}
