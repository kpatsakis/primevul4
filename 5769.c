ImageAddress (void *image, uint64_t size, uint64_t address)
{
	/* ensure our local pointer isn't bigger than our size */
	if (address > size)
		return NULL;

	/* Insure our math won't overflow */
	if (UINT64_MAX - address < (uint64_t)(intptr_t)image)
		return NULL;

	/* return the absolute pointer */
	return image + address;
}