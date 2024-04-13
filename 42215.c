int vhost_get_vq_desc(struct vhost_dev *dev, struct vhost_virtqueue *vq,
		      struct iovec iov[], unsigned int iov_size,
		      unsigned int *out_num, unsigned int *in_num,
		      struct vhost_log *log, unsigned int *log_num)
{
	struct vring_desc desc;
	unsigned int i, head, found = 0;
	u16 last_avail_idx;
	int ret;

	/* Check it isn't doing very strange things with descriptor numbers. */
	last_avail_idx = vq->last_avail_idx;
	if (unlikely(__get_user(vq->avail_idx, &vq->avail->idx))) {
		vq_err(vq, "Failed to access avail idx at %p\n",
		       &vq->avail->idx);
		return -EFAULT;
	}

	if (unlikely((u16)(vq->avail_idx - last_avail_idx) > vq->num)) {
		vq_err(vq, "Guest moved used index from %u to %u",
		       last_avail_idx, vq->avail_idx);
		return -EFAULT;
	}

	/* If there's nothing new since last we looked, return invalid. */
	if (vq->avail_idx == last_avail_idx)
		return vq->num;

	/* Only get avail ring entries after they have been exposed by guest. */
	smp_rmb();

	/* Grab the next descriptor number they're advertising, and increment
	 * the index we've seen. */
	if (unlikely(__get_user(head,
				&vq->avail->ring[last_avail_idx % vq->num]))) {
		vq_err(vq, "Failed to read head: idx %d address %p\n",
		       last_avail_idx,
		       &vq->avail->ring[last_avail_idx % vq->num]);
		return -EFAULT;
	}

	/* If their number is silly, that's an error. */
	if (unlikely(head >= vq->num)) {
		vq_err(vq, "Guest says index %u > %u is available",
		       head, vq->num);
		return -EINVAL;
	}

	/* When we start there are none of either input nor output. */
	*out_num = *in_num = 0;
	if (unlikely(log))
		*log_num = 0;

	i = head;
	do {
		unsigned iov_count = *in_num + *out_num;
		if (unlikely(i >= vq->num)) {
			vq_err(vq, "Desc index is %u > %u, head = %u",
			       i, vq->num, head);
			return -EINVAL;
		}
		if (unlikely(++found > vq->num)) {
			vq_err(vq, "Loop detected: last one at %u "
			       "vq size %u head %u\n",
			       i, vq->num, head);
			return -EINVAL;
		}
		ret = __copy_from_user(&desc, vq->desc + i, sizeof desc);
		if (unlikely(ret)) {
			vq_err(vq, "Failed to get descriptor: idx %d addr %p\n",
			       i, vq->desc + i);
			return -EFAULT;
		}
		if (desc.flags & VRING_DESC_F_INDIRECT) {
			ret = get_indirect(dev, vq, iov, iov_size,
					   out_num, in_num,
					   log, log_num, &desc);
			if (unlikely(ret < 0)) {
				vq_err(vq, "Failure detected "
				       "in indirect descriptor at idx %d\n", i);
				return ret;
			}
			continue;
		}

		ret = translate_desc(dev, desc.addr, desc.len, iov + iov_count,
				     iov_size - iov_count);
		if (unlikely(ret < 0)) {
			vq_err(vq, "Translation failure %d descriptor idx %d\n",
			       ret, i);
			return ret;
		}
		if (desc.flags & VRING_DESC_F_WRITE) {
			/* If this is an input descriptor,
			 * increment that count. */
			*in_num += ret;
			if (unlikely(log)) {
				log[*log_num].addr = desc.addr;
				log[*log_num].len = desc.len;
				++*log_num;
			}
		} else {
			/* If it's an output descriptor, they're all supposed
			 * to come before any input descriptors. */
			if (unlikely(*in_num)) {
				vq_err(vq, "Descriptor has out after in: "
				       "idx %d\n", i);
				return -EINVAL;
			}
			*out_num += ret;
		}
	} while ((i = next_desc(&desc)) != -1);

	/* On success, increment avail index. */
	vq->last_avail_idx++;

	/* Assume notifications from guest are disabled at this point,
	 * if they aren't we would need to update avail_event index. */
	BUG_ON(!(vq->used_flags & VRING_USED_F_NO_NOTIFY));
	return head;
}
