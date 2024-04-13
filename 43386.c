static int snd_ctl_elem_list(struct snd_card *card,
			     struct snd_ctl_elem_list __user *_list)
{
	struct list_head *plist;
	struct snd_ctl_elem_list list;
	struct snd_kcontrol *kctl;
	struct snd_ctl_elem_id *dst, *id;
	unsigned int offset, space, jidx;
	
	if (copy_from_user(&list, _list, sizeof(list)))
		return -EFAULT;
	offset = list.offset;
	space = list.space;
	/* try limit maximum space */
	if (space > 16384)
		return -ENOMEM;
	if (space > 0) {
		/* allocate temporary buffer for atomic operation */
		dst = vmalloc(space * sizeof(struct snd_ctl_elem_id));
		if (dst == NULL)
			return -ENOMEM;
		down_read(&card->controls_rwsem);
		list.count = card->controls_count;
		plist = card->controls.next;
		while (plist != &card->controls) {
			if (offset == 0)
				break;
			kctl = snd_kcontrol(plist);
			if (offset < kctl->count)
				break;
			offset -= kctl->count;
			plist = plist->next;
		}
		list.used = 0;
		id = dst;
		while (space > 0 && plist != &card->controls) {
			kctl = snd_kcontrol(plist);
			for (jidx = offset; space > 0 && jidx < kctl->count; jidx++) {
				snd_ctl_build_ioff(id, kctl, jidx);
				id++;
				space--;
				list.used++;
			}
			plist = plist->next;
			offset = 0;
		}
		up_read(&card->controls_rwsem);
		if (list.used > 0 &&
		    copy_to_user(list.pids, dst,
				 list.used * sizeof(struct snd_ctl_elem_id))) {
			vfree(dst);
			return -EFAULT;
		}
		vfree(dst);
	} else {
		down_read(&card->controls_rwsem);
		list.count = card->controls_count;
		up_read(&card->controls_rwsem);
	}
	if (copy_to_user(_list, &list, sizeof(list)))
		return -EFAULT;
	return 0;
}
