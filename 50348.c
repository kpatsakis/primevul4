static struct ctl_node *first_usable_entry(struct rb_node *node)
{
	struct ctl_node *ctl_node;

	for (;node; node = rb_next(node)) {
		ctl_node = rb_entry(node, struct ctl_node, node);
		if (use_table(ctl_node->header))
			return ctl_node;
	}
	return NULL;
}
