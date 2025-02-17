static struct crypto_template *__crypto_lookup_template(const char *name)
{
	struct crypto_template *q, *tmpl = NULL;

	down_read(&crypto_alg_sem);
	list_for_each_entry(q, &crypto_template_list, list) {
		if (strcmp(q->name, name))
			continue;
		if (unlikely(!crypto_tmpl_get(q)))
			continue;

		tmpl = q;
		break;
	}
	up_read(&crypto_alg_sem);

	return tmpl;
}
