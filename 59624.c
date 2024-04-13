ZEND_API void zend_hash_bucket_renum_swap(Bucket *p, Bucket *q)
{
	zval val;

	ZVAL_COPY_VALUE(&val, &p->val);
	ZVAL_COPY_VALUE(&p->val, &q->val);
	ZVAL_COPY_VALUE(&q->val, &val);
}
