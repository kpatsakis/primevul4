static void tg3_frag_free(bool is_frag, void *data)
{
	if (is_frag)
		put_page(virt_to_head_page(data));
	else
		kfree(data);
}
