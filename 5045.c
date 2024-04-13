static void sort_parity_stripes(struct btrfs_bio *bbio, int num_stripes)
{
	int i;
	int again = 1;

	while (again) {
		again = 0;
		for (i = 0; i < num_stripes - 1; i++) {
			/* Swap if parity is on a smaller index */
			if (bbio->raid_map[i] > bbio->raid_map[i + 1]) {
				swap(bbio->stripes[i], bbio->stripes[i + 1]);
				swap(bbio->raid_map[i], bbio->raid_map[i + 1]);
				again = 1;
			}
		}
	}
}