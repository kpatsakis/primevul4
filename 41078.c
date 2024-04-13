static void dx_show_index(char * label, struct dx_entry *entries)
{
	int i, n = dx_get_count (entries);
	printk(KERN_DEBUG "%s index ", label);
	for (i = 0; i < n; i++) {
		printk("%x->%lu ", i ? dx_get_hash(entries + i) :
				0, (unsigned long)dx_get_block(entries + i));
	}
	printk("\n");
}
