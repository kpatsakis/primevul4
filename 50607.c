brcmf_parse_vndr_ies(const u8 *vndr_ie_buf, u32 vndr_ie_len,
		     struct parsed_vndr_ies *vndr_ies)
{
	struct brcmf_vs_tlv *vndrie;
	struct brcmf_tlv *ie;
	struct parsed_vndr_ie_info *parsed_info;
	s32 remaining_len;

	remaining_len = (s32)vndr_ie_len;
	memset(vndr_ies, 0, sizeof(*vndr_ies));

	ie = (struct brcmf_tlv *)vndr_ie_buf;
	while (ie) {
		if (ie->id != WLAN_EID_VENDOR_SPECIFIC)
			goto next;
		vndrie = (struct brcmf_vs_tlv *)ie;
		/* len should be bigger than OUI length + one */
		if (vndrie->len < (VS_IE_FIXED_HDR_LEN - TLV_HDR_LEN + 1)) {
			brcmf_err("invalid vndr ie. length is too small %d\n",
				  vndrie->len);
			goto next;
		}
		/* if wpa or wme ie, do not add ie */
		if (!memcmp(vndrie->oui, (u8 *)WPA_OUI, TLV_OUI_LEN) &&
		    ((vndrie->oui_type == WPA_OUI_TYPE) ||
		    (vndrie->oui_type == WME_OUI_TYPE))) {
			brcmf_dbg(TRACE, "Found WPA/WME oui. Do not add it\n");
			goto next;
		}

		parsed_info = &vndr_ies->ie_info[vndr_ies->count];

		/* save vndr ie information */
		parsed_info->ie_ptr = (char *)vndrie;
		parsed_info->ie_len = vndrie->len + TLV_HDR_LEN;
		memcpy(&parsed_info->vndrie, vndrie, sizeof(*vndrie));

		vndr_ies->count++;

		brcmf_dbg(TRACE, "** OUI %02x %02x %02x, type 0x%02x\n",
			  parsed_info->vndrie.oui[0],
			  parsed_info->vndrie.oui[1],
			  parsed_info->vndrie.oui[2],
			  parsed_info->vndrie.oui_type);

		if (vndr_ies->count >= VNDR_IE_PARSE_LIMIT)
			break;
next:
		remaining_len -= (ie->len + TLV_HDR_LEN);
		if (remaining_len <= TLV_HDR_LEN)
			ie = NULL;
		else
			ie = (struct brcmf_tlv *)(((u8 *)ie) + ie->len +
				TLV_HDR_LEN);
	}
	return 0;
}
