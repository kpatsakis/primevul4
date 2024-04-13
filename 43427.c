static int get_slot_from_bitmask(int mask, int (*check)(struct module *, int),
				 struct module *module)
{
	int slot;

	for (slot = 0; slot < SNDRV_CARDS; slot++) {
		if (slot < 32 && !(mask & (1U << slot)))
			continue;
		if (!test_bit(slot, snd_cards_lock)) {
			if (check(module, slot))
				return slot; /* found */
		}
	}
	return mask; /* unchanged */
}
