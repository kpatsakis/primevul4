static struct ctl_table_header *sysctl_head_grab(struct ctl_table_header *head)
{
	BUG_ON(!head);
	spin_lock(&sysctl_lock);
	if (!use_table(head))
		head = ERR_PTR(-ENOENT);
	spin_unlock(&sysctl_lock);
	return head;
}
