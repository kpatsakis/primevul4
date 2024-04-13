static void tell_sector(void)
{
	pr_cont(": track %d, head %d, sector %d, size %d",
		R_TRACK, R_HEAD, R_SECTOR, R_SIZECODE);
}				/* tell_sector */
