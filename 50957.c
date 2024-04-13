static int _ffs_func_bind(struct usb_configuration *c,
			  struct usb_function *f)
{
	struct ffs_function *func = ffs_func_from_usb(f);
	struct ffs_data *ffs = func->ffs;

	const int full = !!func->ffs->fs_descs_count;
	const int high = gadget_is_dualspeed(func->gadget) &&
		func->ffs->hs_descs_count;
	const int super = gadget_is_superspeed(func->gadget) &&
		func->ffs->ss_descs_count;

	int fs_len, hs_len, ss_len, ret, i;

	/* Make it a single chunk, less management later on */
	vla_group(d);
	vla_item_with_sz(d, struct ffs_ep, eps, ffs->eps_count);
	vla_item_with_sz(d, struct usb_descriptor_header *, fs_descs,
		full ? ffs->fs_descs_count + 1 : 0);
	vla_item_with_sz(d, struct usb_descriptor_header *, hs_descs,
		high ? ffs->hs_descs_count + 1 : 0);
	vla_item_with_sz(d, struct usb_descriptor_header *, ss_descs,
		super ? ffs->ss_descs_count + 1 : 0);
	vla_item_with_sz(d, short, inums, ffs->interfaces_count);
	vla_item_with_sz(d, struct usb_os_desc_table, os_desc_table,
			 c->cdev->use_os_string ? ffs->interfaces_count : 0);
	vla_item_with_sz(d, char[16], ext_compat,
			 c->cdev->use_os_string ? ffs->interfaces_count : 0);
	vla_item_with_sz(d, struct usb_os_desc, os_desc,
			 c->cdev->use_os_string ? ffs->interfaces_count : 0);
	vla_item_with_sz(d, struct usb_os_desc_ext_prop, ext_prop,
			 ffs->ms_os_descs_ext_prop_count);
	vla_item_with_sz(d, char, ext_prop_name,
			 ffs->ms_os_descs_ext_prop_name_len);
	vla_item_with_sz(d, char, ext_prop_data,
			 ffs->ms_os_descs_ext_prop_data_len);
	vla_item_with_sz(d, char, raw_descs, ffs->raw_descs_length);
	char *vlabuf;

	ENTER();

	/* Has descriptors only for speeds gadget does not support */
	if (unlikely(!(full | high | super)))
		return -ENOTSUPP;

	/* Allocate a single chunk, less management later on */
	vlabuf = kzalloc(vla_group_size(d), GFP_KERNEL);
	if (unlikely(!vlabuf))
		return -ENOMEM;

	ffs->ms_os_descs_ext_prop_avail = vla_ptr(vlabuf, d, ext_prop);
	ffs->ms_os_descs_ext_prop_name_avail =
		vla_ptr(vlabuf, d, ext_prop_name);
	ffs->ms_os_descs_ext_prop_data_avail =
		vla_ptr(vlabuf, d, ext_prop_data);

	/* Copy descriptors  */
	memcpy(vla_ptr(vlabuf, d, raw_descs), ffs->raw_descs,
	       ffs->raw_descs_length);

	memset(vla_ptr(vlabuf, d, inums), 0xff, d_inums__sz);
	for (ret = ffs->eps_count; ret; --ret) {
		struct ffs_ep *ptr;

		ptr = vla_ptr(vlabuf, d, eps);
		ptr[ret].num = -1;
	}

	/* Save pointers
	 * d_eps == vlabuf, func->eps used to kfree vlabuf later
	*/
	func->eps             = vla_ptr(vlabuf, d, eps);
	func->interfaces_nums = vla_ptr(vlabuf, d, inums);

	/*
	 * Go through all the endpoint descriptors and allocate
	 * endpoints first, so that later we can rewrite the endpoint
	 * numbers without worrying that it may be described later on.
	 */
	if (likely(full)) {
		func->function.fs_descriptors = vla_ptr(vlabuf, d, fs_descs);
		fs_len = ffs_do_descs(ffs->fs_descs_count,
				      vla_ptr(vlabuf, d, raw_descs),
				      d_raw_descs__sz,
				      __ffs_func_bind_do_descs, func);
		if (unlikely(fs_len < 0)) {
			ret = fs_len;
			goto error;
		}
	} else {
		fs_len = 0;
	}

	if (likely(high)) {
		func->function.hs_descriptors = vla_ptr(vlabuf, d, hs_descs);
		hs_len = ffs_do_descs(ffs->hs_descs_count,
				      vla_ptr(vlabuf, d, raw_descs) + fs_len,
				      d_raw_descs__sz - fs_len,
				      __ffs_func_bind_do_descs, func);
		if (unlikely(hs_len < 0)) {
			ret = hs_len;
			goto error;
		}
	} else {
		hs_len = 0;
	}

	if (likely(super)) {
		func->function.ss_descriptors = vla_ptr(vlabuf, d, ss_descs);
		ss_len = ffs_do_descs(ffs->ss_descs_count,
				vla_ptr(vlabuf, d, raw_descs) + fs_len + hs_len,
				d_raw_descs__sz - fs_len - hs_len,
				__ffs_func_bind_do_descs, func);
		if (unlikely(ss_len < 0)) {
			ret = ss_len;
			goto error;
		}
	} else {
		ss_len = 0;
	}

	/*
	 * Now handle interface numbers allocation and interface and
	 * endpoint numbers rewriting.  We can do that in one go
	 * now.
	 */
	ret = ffs_do_descs(ffs->fs_descs_count +
			   (high ? ffs->hs_descs_count : 0) +
			   (super ? ffs->ss_descs_count : 0),
			   vla_ptr(vlabuf, d, raw_descs), d_raw_descs__sz,
			   __ffs_func_bind_do_nums, func);
	if (unlikely(ret < 0))
		goto error;

	func->function.os_desc_table = vla_ptr(vlabuf, d, os_desc_table);
	if (c->cdev->use_os_string)
		for (i = 0; i < ffs->interfaces_count; ++i) {
			struct usb_os_desc *desc;

			desc = func->function.os_desc_table[i].os_desc =
				vla_ptr(vlabuf, d, os_desc) +
				i * sizeof(struct usb_os_desc);
			desc->ext_compat_id =
				vla_ptr(vlabuf, d, ext_compat) + i * 16;
			INIT_LIST_HEAD(&desc->ext_prop);
		}
	ret = ffs_do_os_descs(ffs->ms_os_descs_count,
			      vla_ptr(vlabuf, d, raw_descs) +
			      fs_len + hs_len + ss_len,
			      d_raw_descs__sz - fs_len - hs_len - ss_len,
			      __ffs_func_bind_do_os_desc, func);
	if (unlikely(ret < 0))
		goto error;
	func->function.os_desc_n =
		c->cdev->use_os_string ? ffs->interfaces_count : 0;

	/* And we're done */
	ffs_event_add(ffs, FUNCTIONFS_BIND);
	return 0;

error:
	/* XXX Do we need to release all claimed endpoints here? */
	return ret;
}
