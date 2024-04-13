static int snd_ctl_elem_init_enum_names(struct user_element *ue)
{
	char *names, *p;
	size_t buf_len, name_len;
	unsigned int i;
	const uintptr_t user_ptrval = ue->info.value.enumerated.names_ptr;

	if (ue->info.value.enumerated.names_length > 64 * 1024)
		return -EINVAL;

	names = memdup_user((const void __user *)user_ptrval,
		ue->info.value.enumerated.names_length);
	if (IS_ERR(names))
		return PTR_ERR(names);

	/* check that there are enough valid names */
	buf_len = ue->info.value.enumerated.names_length;
	p = names;
	for (i = 0; i < ue->info.value.enumerated.items; ++i) {
		name_len = strnlen(p, buf_len);
		if (name_len == 0 || name_len >= 64 || name_len == buf_len) {
			kfree(names);
			return -EINVAL;
		}
		p += name_len + 1;
		buf_len -= name_len + 1;
	}

	ue->priv_data = names;
	ue->info.value.enumerated.names_ptr = 0;

	return 0;
}
