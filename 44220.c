static void cm_mask_copy(u8 *dst, u8 *src, u8 *mask)
{
	int i;

	for (i = 0; i < IB_CM_COMPARE_SIZE / sizeof(unsigned long); i++)
		((unsigned long *) dst)[i] = ((unsigned long *) src)[i] &
					     ((unsigned long *) mask)[i];
}
