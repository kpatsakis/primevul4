size_t csum_and_copy_to_iter(const void *addr, size_t bytes, __wsum *csum,
			     struct iov_iter *i)
{
	const char *from = addr;
	__wsum sum, next;
	size_t off = 0;
	sum = *csum;
	if (unlikely(i->type & ITER_PIPE)) {
		WARN_ON(1);	/* for now */
		return 0;
	}
	iterate_and_advance(i, bytes, v, ({
		int err = 0;
		next = csum_and_copy_to_user((from += v.iov_len) - v.iov_len,
					     v.iov_base,
					     v.iov_len, 0, &err);
		if (!err) {
			sum = csum_block_add(sum, next, off);
			off += v.iov_len;
		}
		err ? v.iov_len : 0;
	}), ({
		char *p = kmap_atomic(v.bv_page);
		next = csum_partial_copy_nocheck((from += v.bv_len) - v.bv_len,
						 p + v.bv_offset,
						 v.bv_len, 0);
		kunmap_atomic(p);
		sum = csum_block_add(sum, next, off);
		off += v.bv_len;
	}),({
		next = csum_partial_copy_nocheck((from += v.iov_len) - v.iov_len,
						 v.iov_base,
						 v.iov_len, 0);
		sum = csum_block_add(sum, next, off);
		off += v.iov_len;
	})
	)
	*csum = sum;
	return bytes;
}
