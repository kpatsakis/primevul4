static inline struct ias_object *irias_seq_idx(loff_t pos)
{
	struct ias_object *obj;

	for (obj = (struct ias_object *) hashbin_get_first(irias_objects);
	     obj; obj = (struct ias_object *) hashbin_get_next(irias_objects)) {
		if (pos-- == 0)
			break;
	}

	return obj;
}
