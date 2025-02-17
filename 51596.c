heap_get_entry(struct heap_queue *heap)
{
	uint64_t a_key, b_key, c_key;
	int a, b, c;
	struct file_info *r, *tmp;

	if (heap->used < 1)
		return (NULL);

	/*
	 * The first file in the list is the earliest; we'll return this.
	 */
	r = heap->files[0];

	/*
	 * Move the last item in the heap to the root of the tree
	 */
	heap->files[0] = heap->files[--(heap->used)];

	/*
	 * Rebalance the heap.
	 */
	a = 0; /* Starting element and its heap key */
	a_key = heap->files[a]->key;
	for (;;) {
		b = a + a + 1; /* First child */
		if (b >= heap->used)
			return (r);
		b_key = heap->files[b]->key;
		c = b + 1; /* Use second child if it is smaller. */
		if (c < heap->used) {
			c_key = heap->files[c]->key;
			if (c_key < b_key) {
				b = c;
				b_key = c_key;
			}
		}
		if (a_key <= b_key)
			return (r);
		tmp = heap->files[a];
		heap->files[a] = heap->files[b];
		heap->files[b] = tmp;
		a = b;
	}
}
