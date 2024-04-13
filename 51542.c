__audit_reusename(const __user char *uptr)
{
	struct audit_context *context = current->audit_context;
	struct audit_names *n;

	list_for_each_entry(n, &context->names_list, list) {
		if (!n->name)
			continue;
		if (n->name->uptr == uptr) {
			n->name->refcnt++;
			return n->name;
		}
	}
	return NULL;
}
