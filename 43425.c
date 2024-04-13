static int check_empty_slot(struct module *module, int slot)
{
	return !slots[slot] || !*slots[slot];
}
