static void *realloc_array(void *arr, size_t old_n, size_t new_n, size_t size)
{
	if (!new_n || old_n == new_n)
		goto out;

	arr = krealloc_array(arr, new_n, size, GFP_KERNEL);
	if (!arr)
		return NULL;

	if (new_n > old_n)
		memset(arr + old_n * size, 0, (new_n - old_n) * size);

out:
	return arr ? arr : ZERO_SIZE_PTR;
}