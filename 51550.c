static struct audit_names *audit_alloc_name(struct audit_context *context,
						unsigned char type)
{
	struct audit_names *aname;

	if (context->name_count < AUDIT_NAMES) {
		aname = &context->preallocated_names[context->name_count];
		memset(aname, 0, sizeof(*aname));
	} else {
		aname = kzalloc(sizeof(*aname), GFP_NOFS);
		if (!aname)
			return NULL;
		aname->should_free = true;
	}

	aname->ino = AUDIT_INO_UNSET;
	aname->type = type;
	list_add_tail(&aname->list, &context->names_list);

	context->name_count++;
	return aname;
}
