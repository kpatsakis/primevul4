static int kvm_create_dirty_bitmap(struct kvm_memory_slot *memslot)
{
	unsigned long dirty_bytes = 2 * kvm_dirty_bitmap_bytes(memslot);

	if (dirty_bytes > PAGE_SIZE)
		memslot->dirty_bitmap = vzalloc(dirty_bytes);
	else
		memslot->dirty_bitmap = kzalloc(dirty_bytes, GFP_KERNEL);

	if (!memslot->dirty_bitmap)
		return -ENOMEM;

	memslot->dirty_bitmap_head = memslot->dirty_bitmap;
	return 0;
}
