static void *vhost_kvzalloc(unsigned long size)
{
	void *n = kzalloc(size, GFP_KERNEL | __GFP_NOWARN | __GFP_REPEAT);

	if (!n) {
		n = vzalloc(size);
		if (!n)
			return ERR_PTR(-ENOMEM);
	}
	return n;
}
