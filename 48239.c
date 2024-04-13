static struct list_head *crypto_more_spawns(struct crypto_alg *alg,
					    struct list_head *stack,
					    struct list_head *top,
					    struct list_head *secondary_spawns)
{
	struct crypto_spawn *spawn, *n;

	if (list_empty(stack))
		return NULL;

	spawn = list_first_entry(stack, struct crypto_spawn, list);
	n = list_entry(spawn->list.next, struct crypto_spawn, list);

	if (spawn->alg && &n->list != stack && !n->alg)
		n->alg = (n->list.next == stack) ? alg :
			 &list_entry(n->list.next, struct crypto_spawn,
				     list)->inst->alg;

	list_move(&spawn->list, secondary_spawns);

	return &n->list == stack ? top : &n->inst->alg.cra_users;
}
