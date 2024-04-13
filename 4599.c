GF_EXPORT
GF_Err gf_isom_dump_supported_box(u32 idx, FILE * trace)
{
	u32 i;
	u32 nb_versions=0;
	GF_Err e;

	if (box_registry[idx].max_version_plus_one) {
		nb_versions = box_registry[idx].max_version_plus_one - 1;
	}
	for (i = 0; i <= nb_versions; i++) {
		GF_Box *a = box_registry[idx].new_fn();
		if (!a) return GF_OUT_OF_MEM;

		a->registry = &box_registry[idx];

		if (box_registry[idx].alt_4cc) {
			if (a->type==GF_ISOM_BOX_TYPE_REFT)
				((GF_TrackReferenceTypeBox*)a)->reference_type = box_registry[idx].alt_4cc;
			else if (a->type==GF_ISOM_BOX_TYPE_REFI)
				((GF_ItemReferenceTypeBox*)a)->reference_type = box_registry[idx].alt_4cc;
			else if (a->type==GF_ISOM_BOX_TYPE_TRGT)
				((GF_TrackGroupTypeBox*)a)->group_type = box_registry[idx].alt_4cc;
			else if (a->type==GF_ISOM_BOX_TYPE_SGPD)
				((GF_SampleGroupDescriptionBox*)a)->grouping_type = box_registry[idx].alt_4cc;
			else if (a->type==GF_ISOM_BOX_TYPE_GRPT)
				((GF_EntityToGroupTypeBox*)a)->grouping_type = box_registry[idx].alt_4cc;
		}
		if (box_registry[idx].max_version_plus_one) {
			((GF_FullBox *)a)->version = i;
		}
		if (box_registry[idx].flags) {
			u32 flag_mask=1;
			u32 flags = box_registry[idx].flags;
			((GF_FullBox *)a)->flags = 0;
			e = gf_isom_box_dump(a, trace);

			//we dump all flags individually and this for all version, in order to simplify the XSLT processing
			while (!e) {
				u32 flag = flags & flag_mask;
				flag_mask <<= 1;
				if (flag) {
					((GF_FullBox *)a)->flags = flag;
					e = gf_isom_box_dump(a, trace);
				}
				if (flag_mask > flags) break;
				if (flag_mask == 0x80000000) break;
			}

		} else {
			e = gf_isom_box_dump(a, trace);
		}

		gf_isom_box_del(a);
	}
	return e;