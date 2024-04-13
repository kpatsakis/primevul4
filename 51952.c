static void write_volume_label(DOS_FS * fs, char *label)
{
    time_t now = time(NULL);
    struct tm *mtime = localtime(&now);
    off_t offset;
    int created;
    DIR_ENT de;

    created = 0;
    offset = find_volume_de(fs, &de);
    if (offset == 0) {
	created = 1;
	offset = alloc_rootdir_entry(fs, &de, label);
    }
    memcpy(de.name, label, 11);
    de.time = htole16((unsigned short)((mtime->tm_sec >> 1) +
				       (mtime->tm_min << 5) +
				       (mtime->tm_hour << 11)));
    de.date = htole16((unsigned short)(mtime->tm_mday +
				       ((mtime->tm_mon + 1) << 5) +
				       ((mtime->tm_year - 80) << 9)));
    if (created) {
	de.attr = ATTR_VOLUME;
	de.ctime_ms = 0;
	de.ctime = de.time;
	de.cdate = de.date;
	de.adate = de.date;
	de.starthi = 0;
	de.start = 0;
	de.size = 0;
    }

    fs_write(offset, sizeof(DIR_ENT), &de);
}
