heap_get_entry(struct heap_queue *heap)
{
	uint64_t a_id, b_id, c_id;
	int a, b, c;
	struct xar_file *r, *tmp;

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
	a_id = heap->files[a]->id;
	for (;;) {
		b = a + a + 1; /* First child */
		if (b >= heap->used)
			return (r);
		b_id = heap->files[b]->id;
		c = b + 1; /* Use second child if it is smaller. */
		if (c < heap->used) {
			c_id = heap->files[c]->id;
			if (c_id < b_id) {
				b = c;
				b_id = c_id;
			}
		}
		if (a_id <= b_id)
			return (r);
		tmp = heap->files[a];
		heap->files[a] = heap->files[b];
		heap->files[b] = tmp;
		a = b;
	}
}
