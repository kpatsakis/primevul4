badblocks_store(struct badblocks *bb, const char *page, size_t len, int unack)
{
	unsigned long long sector;
	int length;
	char newline;
#ifdef DO_DEBUG
	/* Allow clearing via sysfs *only* for testing/debugging.
	 * Normally only a successful write may clear a badblock
	 */
	int clear = 0;
	if (page[0] == '-') {
		clear = 1;
		page++;
	}
#endif /* DO_DEBUG */

	switch (sscanf(page, "%llu %d%c", &sector, &length, &newline)) {
	case 3:
		if (newline != '\n')
			return -EINVAL;
	case 2:
		if (length <= 0)
			return -EINVAL;
		break;
	default:
		return -EINVAL;
	}

#ifdef DO_DEBUG
	if (clear) {
		md_clear_badblocks(bb, sector, length);
		return len;
	}
#endif /* DO_DEBUG */
	if (md_set_badblocks(bb, sector, length, !unack))
		return len;
	else
		return -ENOSPC;
}
