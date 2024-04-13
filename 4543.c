
GF_Box *gf_isom_box_new_ex(u32 boxType, u32 parentType, Bool skip_logs, Bool is_root_box)
{
	GF_Box *a;
	s32 idx = get_box_reg_idx(boxType, parentType, 0);
	if (idx==0) {
#ifndef GPAC_DISABLE_LOG
		if (!skip_logs && (boxType != GF_ISOM_BOX_TYPE_UNKNOWN) && (boxType != GF_ISOM_BOX_TYPE_UUID)) {
			switch (parentType) {
			case GF_ISOM_BOX_TYPE_ILST:
			case GF_ISOM_BOX_TYPE_META:
			case GF_ISOM_BOX_TYPE_UDTA:
			case GF_ISOM_BOX_TYPE_UNKNOWN:
			case GF_ISOM_BOX_TYPE_iTunesSpecificInfo:
			case GF_QT_BOX_TYPE_WAVE:
				break;
			//some sample descritions are handled as generic ones but we know them, don't warn
			case GF_ISOM_BOX_TYPE_STSD:
				if (boxType==GF_ISOM_SUBTYPE_FFV1) break;
				//fallthrough
			default:
				if (boxType==GF_ISOM_BOX_TYPE_GDAT) break;

				if (is_root_box) {
					GF_LOG(GF_LOG_INFO, GF_LOG_CONTAINER, ("[iso file] Unknown top-level box type %s\n", gf_4cc_to_str(boxType)));
				} else if (parentType) {
					char szName[GF_4CC_MSIZE];
					strcpy(szName, gf_4cc_to_str(parentType));
					GF_LOG(GF_LOG_INFO, GF_LOG_CONTAINER, ("[iso file] Unknown box type %s in parent %s\n", gf_4cc_to_str(boxType), szName));
				} else {
					GF_LOG(GF_LOG_INFO, GF_LOG_CONTAINER, ("[iso file] Unknown box type %s\n", gf_4cc_to_str(boxType)));
				}
				break;
			}
		}
#endif
        if (boxType==GF_ISOM_BOX_TYPE_UUID) {
            a = uuid_box_new();
            if (a) a->registry = &box_registry[1];
        } else {
            a = unkn_box_new();
            if (a) {
            	((GF_UnknownBox *)a)->original_4cc = boxType;
            	a->registry = &box_registry[0];
			}
        }
		return a;
	}
	a = box_registry[idx].new_fn();

	if (a) {
		if (a->type!=GF_ISOM_BOX_TYPE_UUID) {
			if (a->type==GF_ISOM_BOX_TYPE_UNKNOWN) {
				((GF_UnknownBox *)a)->original_4cc = boxType;
			} else {
				a->type = boxType;
			}
		}
		a->registry = &box_registry[idx];

		if ((a->type==GF_ISOM_BOX_TYPE_COLR) && (parentType==GF_ISOM_BOX_TYPE_JP2H)) {
			((GF_ColourInformationBox *)a)->is_jp2 = GF_TRUE;
		}

	}
	return a;