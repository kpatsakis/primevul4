void __key_check(const struct key *key)
{
	printk("__key_check: key %p {%08x} should be {%08x}\n",
	       key, key->magic, KEY_DEBUG_MAGIC);
	BUG();
}
