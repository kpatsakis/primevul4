GF_Err dims_on_child_box(GF_Box *s, GF_Box *a, Bool is_rem)
{
	GF_DIMSSampleEntryBox *ptr = (GF_DIMSSampleEntryBox  *)s;
	switch (a->type) {
	case GF_ISOM_BOX_TYPE_DIMC:
		BOX_FIELD_ASSIGN(config, GF_DIMSSceneConfigBox)
		break;
	case GF_ISOM_BOX_TYPE_DIST:
		BOX_FIELD_ASSIGN(scripts, GF_DIMSScriptTypesBox)
		break;
	}
	return GF_OK;
}