static void ion_buffer_get(struct ion_buffer *buffer)
{
	kref_get(&buffer->ref);
}
