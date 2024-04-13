static void erase_entry(struct ctl_table_header *head, struct ctl_table *entry)
{
	struct rb_node *node = &head->node[entry - head->ctl_table].node;

	rb_erase(node, &head->parent->root);
}
