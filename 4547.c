static void convert_compact_sample_groups(GF_List *child_boxes, GF_List *sampleGroups)
{
	u32 i;
	for (i=0; i<gf_list_count(sampleGroups); i++) {
		u32 j;
		GF_SampleGroupBox *sbgp;
		GF_CompactSampleGroupBox *csgp = gf_list_get(sampleGroups, i);
		if (csgp->type != GF_ISOM_BOX_TYPE_CSGP) continue;

		gf_list_rem(sampleGroups, i);
		gf_list_del_item(child_boxes, csgp);

		sbgp = (GF_SampleGroupBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_SBGP);
		gf_list_insert(sampleGroups, sbgp, i);
		gf_list_add(child_boxes, sbgp);
		i--;

		sbgp->grouping_type = csgp->grouping_type;
		if (csgp->grouping_type_parameter) {
			sbgp->grouping_type_parameter = csgp->grouping_type_parameter;
			sbgp->version = 1;
		}
		sbgp->entry_count = 0;
		for (j=0; j<csgp->pattern_count; j++) {
			u32 k=0;
			u32 nb_samples = csgp->patterns[j].sample_count;
			//unroll the pattern
			while (nb_samples) {
				u32 nb_same_index=1;
				if (csgp->patterns[j].length<=k)
					break;
				u32 sg_idx = csgp->patterns[j].sample_group_description_indices[k];
				while (nb_same_index+k<csgp->patterns[j].length) {
					if (csgp->patterns[j].sample_group_description_indices[k+nb_same_index] != sg_idx)
						break;
					nb_same_index++;
				}
				sbgp->sample_entries = gf_realloc(sbgp->sample_entries, sizeof(GF_SampleGroupEntry) * (sbgp->entry_count+1));
				if (nb_same_index>nb_samples)
					nb_same_index = nb_samples;

				sbgp->sample_entries[sbgp->entry_count].sample_count = nb_same_index;
				sbgp->sample_entries[sbgp->entry_count].group_description_index = sg_idx;
				nb_samples -= nb_same_index;
				sbgp->entry_count++;
				k+= nb_same_index;
				if (k==csgp->patterns[j].length)
					k = 0;
			}
		}
		gf_isom_box_del((GF_Box*)csgp);
	}
}