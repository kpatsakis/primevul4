static u16 map_class(u16 pol_value)
{
	u16 i;

	for (i = 1; i < current_mapping_size; i++) {
		if (current_mapping[i].value == pol_value)
			return i;
	}

	return SECCLASS_NULL;
}
