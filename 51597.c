isBootRecord(struct iso9660 *iso9660, const unsigned char *h)
{
	(void)iso9660; /* UNUSED */

	/* Type of the Volume Descriptor Boot Record must be 0. */
	if (h[0] != 0)
		return (0);

	/* Volume Descriptor Version must be 1. */
	if (h[6] != 1)
		return (0);

	return (1);
}
