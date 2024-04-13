static int find_highest_prio_tid(unsigned long tids)
{
	/* lower 3 TIDs aren't ordered perfectly */
	if (tids & 0xF8)
		return fls(tids) - 1;
	/* TID 0 is BE just like TID 3 */
	if (tids & BIT(0))
		return 0;
	return fls(tids) - 1;
}
