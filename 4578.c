GF_EdtsEntry *CreateEditEntry(u64 EditDuration, u64 MediaTime, u32 MediaRate, u8 EditMode)
{
	GF_EdtsEntry *ent;

	ent = (GF_EdtsEntry*)gf_malloc(sizeof(GF_EdtsEntry));
	if (!ent) return NULL;

	switch (EditMode) {
	case GF_ISOM_EDIT_EMPTY:
		ent->mediaRate = 0x10000;
		ent->mediaTime = -1;
		break;

	case GF_ISOM_EDIT_DWELL:
		ent->mediaRate = 0;
		ent->mediaTime = MediaTime;
		break;
	default:
		ent->mediaRate = MediaRate;
		ent->mediaTime = MediaTime;
		break;
	}
	ent->segmentDuration = EditDuration;
	return ent;
}