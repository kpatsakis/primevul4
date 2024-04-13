
GF_Err gf_isom_box_dump_start(GF_Box *a, const char *name, FILE * trace)
{
	gf_fprintf(trace, "<%s ", name);
	if (a->size > 0xFFFFFFFF) {
		gf_fprintf(trace, "LargeSize=\""LLU"\" ", a->size);
	} else {
		gf_fprintf(trace, "Size=\"%u\" ", (u32) a->size);
	}
	if (a->type==GF_ISOM_BOX_TYPE_UNKNOWN) {
		gf_fprintf(trace, "Type=\"%s\" ", gf_4cc_to_str(((GF_UnknownBox*)a)->original_4cc));
	} else {
		gf_fprintf(trace, "Type=\"%s\" ", gf_4cc_to_str(a->type));
	}

	if (a->type == GF_ISOM_BOX_TYPE_UUID) {
		u32 i;
		gf_fprintf(trace, "UUID=\"{");
		for (i=0; i<16; i++) {
			gf_fprintf(trace, "%02X", (unsigned char) ((GF_UUIDBox*)a)->uuid[i]);
			if ((i<15) && (i%4)==3) gf_fprintf(trace, "-");
		}
		gf_fprintf(trace, "}\" ");
	}

	if (a->registry->max_version_plus_one) {
		gf_fprintf(trace, "Version=\"%d\" Flags=\"%d\" ", ((GF_FullBox*)a)->version,((GF_FullBox*)a)->flags);
	}
	gf_fprintf(trace, "Specification=\"%s\" ", a->registry->spec);

	//don't write containers in test mode, that would require rewriting hashes whenever spec changes
	if (!gf_sys_is_test_mode()) {
		gf_fprintf(trace, "Container=\"%s\" ", a->registry->parents_4cc);
	}
	return GF_OK;