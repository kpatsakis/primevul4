static int skb_copy_and_csum_datagram(const struct sk_buff *skb, int offset,
				      u8 __user *to, int len,
				      __wsum *csump)
{
	int start = skb_headlen(skb);
	int i, copy = start - offset;
	struct sk_buff *frag_iter;
	int pos = 0;

	/* Copy header. */
	if (copy > 0) {
		int err = 0;
		if (copy > len)
			copy = len;
		*csump = csum_and_copy_to_user(skb->data + offset, to, copy,
					       *csump, &err);
		if (err)
			goto fault;
		if ((len -= copy) == 0)
			return 0;
		offset += copy;
		to += copy;
		pos = copy;
	}

	for (i = 0; i < skb_shinfo(skb)->nr_frags; i++) {
		int end;
		const skb_frag_t *frag = &skb_shinfo(skb)->frags[i];

		WARN_ON(start > offset + len);

		end = start + skb_frag_size(frag);
		if ((copy = end - offset) > 0) {
			__wsum csum2;
			int err = 0;
			u8  *vaddr;
			struct page *page = skb_frag_page(frag);

			if (copy > len)
				copy = len;
			vaddr = kmap(page);
			csum2 = csum_and_copy_to_user(vaddr +
							frag->page_offset +
							offset - start,
						      to, copy, 0, &err);
			kunmap(page);
			if (err)
				goto fault;
			*csump = csum_block_add(*csump, csum2, pos);
			if (!(len -= copy))
				return 0;
			offset += copy;
			to += copy;
			pos += copy;
		}
		start = end;
	}

	skb_walk_frags(skb, frag_iter) {
		int end;

		WARN_ON(start > offset + len);

		end = start + frag_iter->len;
		if ((copy = end - offset) > 0) {
			__wsum csum2 = 0;
			if (copy > len)
				copy = len;
			if (skb_copy_and_csum_datagram(frag_iter,
						       offset - start,
						       to, copy,
						       &csum2))
				goto fault;
			*csump = csum_block_add(*csump, csum2, pos);
			if ((len -= copy) == 0)
				return 0;
			offset += copy;
			to += copy;
			pos += copy;
		}
		start = end;
	}
	if (!len)
		return 0;

fault:
	return -EFAULT;
}
