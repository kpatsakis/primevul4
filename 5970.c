GF_Err leva_box_size(GF_Box *s)
{
	u32 i;
	GF_LevelAssignmentBox *ptr = (GF_LevelAssignmentBox*)s;

	ptr->size += 1;
	for (i = 0; i < ptr->level_count; i++) {
		ptr->size += 5;
		if (ptr->levels[i].type == 0 || ptr->levels[i].type == 4) {
			ptr->size += 4;
		}
		else if (ptr->levels[i].type == 1) {
			ptr->size += 8;
		}
	}
	return GF_OK;
}