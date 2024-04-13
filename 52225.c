free_options(struct mtree_option *head)
{
	struct mtree_option *next;

	for (; head != NULL; head = next) {
		next = head->next;
		free(head->value);
		free(head);
	}
}
