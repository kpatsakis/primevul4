static Node *create_entry(const char __user *buffer, size_t count)
{
	Node *e;
	int memsize, err;
	char *buf, *p;
	char del;

	/* some sanity checks */
	err = -EINVAL;
	if ((count < 11) || (count > 256))
		goto out;

	err = -ENOMEM;
	memsize = sizeof(Node) + count + 8;
	e = kmalloc(memsize, GFP_USER);
	if (!e)
		goto out;

	p = buf = (char *)e + sizeof(Node);

	memset(e, 0, sizeof(Node));
	if (copy_from_user(buf, buffer, count))
		goto Efault;

	del = *p++;	/* delimeter */

	memset(buf+count, del, 8);

	e->name = p;
	p = strchr(p, del);
	if (!p)
		goto Einval;
	*p++ = '\0';
	if (!e->name[0] ||
	    !strcmp(e->name, ".") ||
	    !strcmp(e->name, "..") ||
	    strchr(e->name, '/'))
		goto Einval;
	switch (*p++) {
		case 'E': e->flags = 1<<Enabled; break;
		case 'M': e->flags = (1<<Enabled) | (1<<Magic); break;
		default: goto Einval;
	}
	if (*p++ != del)
		goto Einval;
	if (test_bit(Magic, &e->flags)) {
		char *s = strchr(p, del);
		if (!s)
			goto Einval;
		*s++ = '\0';
		e->offset = simple_strtoul(p, &p, 10);
		if (*p++)
			goto Einval;
		e->magic = p;
		p = scanarg(p, del);
		if (!p)
			goto Einval;
		p[-1] = '\0';
		if (!e->magic[0])
			goto Einval;
		e->mask = p;
		p = scanarg(p, del);
		if (!p)
			goto Einval;
		p[-1] = '\0';
		if (!e->mask[0])
			e->mask = NULL;
		e->size = unquote(e->magic);
		if (e->mask && unquote(e->mask) != e->size)
			goto Einval;
		if (e->size + e->offset > BINPRM_BUF_SIZE)
			goto Einval;
	} else {
		p = strchr(p, del);
		if (!p)
			goto Einval;
		*p++ = '\0';
		e->magic = p;
		p = strchr(p, del);
		if (!p)
			goto Einval;
		*p++ = '\0';
		if (!e->magic[0] || strchr(e->magic, '/'))
			goto Einval;
		p = strchr(p, del);
		if (!p)
			goto Einval;
		*p++ = '\0';
	}
	e->interpreter = p;
	p = strchr(p, del);
	if (!p)
		goto Einval;
	*p++ = '\0';
	if (!e->interpreter[0])
		goto Einval;


	p = check_special_flags (p, e);

	if (*p == '\n')
		p++;
	if (p != buf + count)
		goto Einval;
	return e;

out:
	return ERR_PTR(err);

Efault:
	kfree(e);
	return ERR_PTR(-EFAULT);
Einval:
	kfree(e);
	return ERR_PTR(-EINVAL);
}
