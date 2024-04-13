static void erase_header(struct ctl_table_header *head)
{
	struct ctl_table *entry;
	for (entry = head->ctl_table; entry->procname; entry++)
		erase_entry(head, entry);
}
