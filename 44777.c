static void virtualdmabug_workaround(void)
{
	int hard_sectors;
	int end_sector;

	if (CT(COMMAND) == FD_WRITE) {
		COMMAND &= ~0x80;	/* switch off multiple track mode */

		hard_sectors = raw_cmd->length >> (7 + SIZECODE);
		end_sector = SECTOR + hard_sectors - 1;
		if (end_sector > SECT_PER_TRACK) {
			pr_info("too many sectors %d > %d\n",
				end_sector, SECT_PER_TRACK);
			return;
		}
		SECT_PER_TRACK = end_sector;
					/* make sure SECT_PER_TRACK
					 * points to end of transfer */
	}
}
