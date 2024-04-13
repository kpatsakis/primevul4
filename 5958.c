GF_Err trak_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 i;
	GF_TrackBox *ptr = (GF_TrackBox *)s;
	e = gf_isom_box_array_read(s, bs);
	if (e) return e;
	e = gf_isom_check_sample_desc(ptr);
	if (e) return e;

	if (!ptr->Header) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Missing TrackHeaderBox\n"));
		return GF_ISOM_INVALID_FILE;
	}
	if (!ptr->Media) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Missing MediaBox\n"));
		return GF_ISOM_INVALID_FILE;
	}
	if (!ptr->Media->information || !ptr->Media->information->sampleTable) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Invalid MediaBox\n"));
		return GF_ISOM_INVALID_FILE;
	}
	if (!ptr->Media->information->sampleTable->SampleSize || (ptr->Media->information->sampleTable->SampleSize->sampleCount==0)) {
		if (ptr->Header->initial_duration) {
			GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[iso file] Track with no samples but duration defined, ignoring duration\n"));
			ptr->Header->initial_duration = 0;
		}
	}

	for (i=0; i<gf_list_count(ptr->Media->information->sampleTable->child_boxes); i++) {
		GF_Box *a = gf_list_get(ptr->Media->information->sampleTable->child_boxes, i);
		if ((a->type ==GF_ISOM_BOX_TYPE_UUID) && (((GF_UUIDBox *)a)->internal_4cc == GF_ISOM_BOX_UUID_PSEC)) {
			ptr->sample_encryption = (struct __sample_encryption_box *) a;
			break;
		}
		else if (a->type == GF_ISOM_BOX_TYPE_SENC) {
			ptr->sample_encryption = (struct __sample_encryption_box *)a;
			break;
		}
	}
	return e;
}