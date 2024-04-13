static struct alloted_s *append_alloted(struct alloted_s **head, char *name, int n)
{
	struct alloted_s *cur, *al;

	if (head == NULL || name == NULL) {
		fprintf(stderr, "NULL parameters to append_alloted not allowed\n");
		return NULL;
	}

	al = (struct alloted_s *)malloc(sizeof(struct alloted_s));

	if (al == NULL) {
		fprintf(stderr, "Out of memory in append_alloted\n");
		return NULL;
	}

	al->name = strdup(name);

	if (al->name == NULL) {
		free(al);
		return NULL;
	}

	al->allowed = n;
	al->next = NULL;

	if (*head == NULL) {
		*head = al;
		return al;
	}

	cur = *head;
	while (cur->next != NULL)
		cur = cur->next;

	cur->next = al;
	return al;
}
