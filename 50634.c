s32 brcmf_vif_set_mgmt_ie(struct brcmf_cfg80211_vif *vif, s32 pktflag,
			  const u8 *vndr_ie_buf, u32 vndr_ie_len)
{
	struct brcmf_if *ifp;
	struct vif_saved_ie *saved_ie;
	s32 err = 0;
	u8  *iovar_ie_buf;
	u8  *curr_ie_buf;
	u8  *mgmt_ie_buf = NULL;
	int mgmt_ie_buf_len;
	u32 *mgmt_ie_len;
	u32 del_add_ie_buf_len = 0;
	u32 total_ie_buf_len = 0;
	u32 parsed_ie_buf_len = 0;
	struct parsed_vndr_ies old_vndr_ies;
	struct parsed_vndr_ies new_vndr_ies;
	struct parsed_vndr_ie_info *vndrie_info;
	s32 i;
	u8 *ptr;
	int remained_buf_len;

	if (!vif)
		return -ENODEV;
	ifp = vif->ifp;
	saved_ie = &vif->saved_ie;

	brcmf_dbg(TRACE, "bsscfgidx %d, pktflag : 0x%02X\n", ifp->bsscfgidx,
		  pktflag);
	iovar_ie_buf = kzalloc(WL_EXTRA_BUF_MAX, GFP_KERNEL);
	if (!iovar_ie_buf)
		return -ENOMEM;
	curr_ie_buf = iovar_ie_buf;
	switch (pktflag) {
	case BRCMF_VNDR_IE_PRBREQ_FLAG:
		mgmt_ie_buf = saved_ie->probe_req_ie;
		mgmt_ie_len = &saved_ie->probe_req_ie_len;
		mgmt_ie_buf_len = sizeof(saved_ie->probe_req_ie);
		break;
	case BRCMF_VNDR_IE_PRBRSP_FLAG:
		mgmt_ie_buf = saved_ie->probe_res_ie;
		mgmt_ie_len = &saved_ie->probe_res_ie_len;
		mgmt_ie_buf_len = sizeof(saved_ie->probe_res_ie);
		break;
	case BRCMF_VNDR_IE_BEACON_FLAG:
		mgmt_ie_buf = saved_ie->beacon_ie;
		mgmt_ie_len = &saved_ie->beacon_ie_len;
		mgmt_ie_buf_len = sizeof(saved_ie->beacon_ie);
		break;
	case BRCMF_VNDR_IE_ASSOCREQ_FLAG:
		mgmt_ie_buf = saved_ie->assoc_req_ie;
		mgmt_ie_len = &saved_ie->assoc_req_ie_len;
		mgmt_ie_buf_len = sizeof(saved_ie->assoc_req_ie);
		break;
	default:
		err = -EPERM;
		brcmf_err("not suitable type\n");
		goto exit;
	}

	if (vndr_ie_len > mgmt_ie_buf_len) {
		err = -ENOMEM;
		brcmf_err("extra IE size too big\n");
		goto exit;
	}

	/* parse and save new vndr_ie in curr_ie_buff before comparing it */
	if (vndr_ie_buf && vndr_ie_len && curr_ie_buf) {
		ptr = curr_ie_buf;
		brcmf_parse_vndr_ies(vndr_ie_buf, vndr_ie_len, &new_vndr_ies);
		for (i = 0; i < new_vndr_ies.count; i++) {
			vndrie_info = &new_vndr_ies.ie_info[i];
			memcpy(ptr + parsed_ie_buf_len, vndrie_info->ie_ptr,
			       vndrie_info->ie_len);
			parsed_ie_buf_len += vndrie_info->ie_len;
		}
	}

	if (mgmt_ie_buf && *mgmt_ie_len) {
		if (parsed_ie_buf_len && (parsed_ie_buf_len == *mgmt_ie_len) &&
		    (memcmp(mgmt_ie_buf, curr_ie_buf,
			    parsed_ie_buf_len) == 0)) {
			brcmf_dbg(TRACE, "Previous mgmt IE equals to current IE\n");
			goto exit;
		}

		/* parse old vndr_ie */
		brcmf_parse_vndr_ies(mgmt_ie_buf, *mgmt_ie_len, &old_vndr_ies);

		/* make a command to delete old ie */
		for (i = 0; i < old_vndr_ies.count; i++) {
			vndrie_info = &old_vndr_ies.ie_info[i];

			brcmf_dbg(TRACE, "DEL ID : %d, Len: %d , OUI:%02x:%02x:%02x\n",
				  vndrie_info->vndrie.id,
				  vndrie_info->vndrie.len,
				  vndrie_info->vndrie.oui[0],
				  vndrie_info->vndrie.oui[1],
				  vndrie_info->vndrie.oui[2]);

			del_add_ie_buf_len = brcmf_vndr_ie(curr_ie_buf, pktflag,
							   vndrie_info->ie_ptr,
							   vndrie_info->ie_len,
							   "del");
			curr_ie_buf += del_add_ie_buf_len;
			total_ie_buf_len += del_add_ie_buf_len;
		}
	}

	*mgmt_ie_len = 0;
	/* Add if there is any extra IE */
	if (mgmt_ie_buf && parsed_ie_buf_len) {
		ptr = mgmt_ie_buf;

		remained_buf_len = mgmt_ie_buf_len;

		/* make a command to add new ie */
		for (i = 0; i < new_vndr_ies.count; i++) {
			vndrie_info = &new_vndr_ies.ie_info[i];

			/* verify remained buf size before copy data */
			if (remained_buf_len < (vndrie_info->vndrie.len +
							VNDR_IE_VSIE_OFFSET)) {
				brcmf_err("no space in mgmt_ie_buf: len left %d",
					  remained_buf_len);
				break;
			}
			remained_buf_len -= (vndrie_info->ie_len +
					     VNDR_IE_VSIE_OFFSET);

			brcmf_dbg(TRACE, "ADDED ID : %d, Len: %d, OUI:%02x:%02x:%02x\n",
				  vndrie_info->vndrie.id,
				  vndrie_info->vndrie.len,
				  vndrie_info->vndrie.oui[0],
				  vndrie_info->vndrie.oui[1],
				  vndrie_info->vndrie.oui[2]);

			del_add_ie_buf_len = brcmf_vndr_ie(curr_ie_buf, pktflag,
							   vndrie_info->ie_ptr,
							   vndrie_info->ie_len,
							   "add");

			/* save the parsed IE in wl struct */
			memcpy(ptr + (*mgmt_ie_len), vndrie_info->ie_ptr,
			       vndrie_info->ie_len);
			*mgmt_ie_len += vndrie_info->ie_len;

			curr_ie_buf += del_add_ie_buf_len;
			total_ie_buf_len += del_add_ie_buf_len;
		}
	}
	if (total_ie_buf_len) {
		err  = brcmf_fil_bsscfg_data_set(ifp, "vndr_ie", iovar_ie_buf,
						 total_ie_buf_len);
		if (err)
			brcmf_err("vndr ie set error : %d\n", err);
	}

exit:
	kfree(iovar_ie_buf);
	return err;
}
