size_t csum_and_copy_from_iter(void *addr, size_t bytes, __wsum *csum,
			       struct iov_iter *i)
{
	char *to = addr;
	__wsum sum, next;
	size_t off = 0;
	sum = *csum;
	if (unlikely(i->type & ITER_PIPE)) {
		WARN_ON(1);
		return 0;
	}
	iterate_and_advance(i, bytes, v, ({
		int err = 0;
		next = csum_and_copy_from_user(v.iov_base,
					       (to += v.iov_len) - v.iov_len,
					       v.iov_len, 0, &err);
		if (!err) {
			sum = csum_block_add(sum, next, off);
			off += v.iov_len;
		}
		err ? v.iov_len : 0;
	}), ({
		char *p = kmap_atomic(v.bv_page);
		next = csum_partial_copy_nocheck(p + v.bv_offset,
						 (to += v.bv_len) - v.bv_len,
						 v.bv_len, 0);
		kunmap_atomic(p);
		sum = csum_block_add(sum, next, off);
		off += v.bv_len;
	}),({
		next = csum_partial_copy_nocheck(v.iov_base,
						 (to += v.iov_len) - v.iov_len,
						 v.iov_len, 0);
		sum = csum_block_add(sum, next, off);
		off += v.iov_len;
	})
	)
	*csum = sum;
	return bytes;
}
