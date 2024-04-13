vbf_objiterator(void *priv, int flush, const void *ptr, ssize_t len)
{
	struct busyobj *bo;
	ssize_t l;
	const uint8_t *ps = ptr;
	uint8_t *pd;

	(void)flush;
	CAST_OBJ_NOTNULL(bo, priv, BUSYOBJ_MAGIC);

	while (len > 0) {
		l = len;
		if (VFP_GetStorage(bo->vfc, &l, &pd) != VFP_OK)
			return (1);
		if (len < l)
			l = len;
		memcpy(pd, ps, l);
		VFP_Extend(bo->vfc, l);
		ps += l;
		len -= l;
	}
	return (0);
}
