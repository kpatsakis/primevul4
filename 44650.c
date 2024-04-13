static void type_attribute_bounds_av(struct context *scontext,
				     struct context *tcontext,
				     u16 tclass,
				     struct av_decision *avd)
{
	struct context lo_scontext;
	struct context lo_tcontext;
	struct av_decision lo_avd;
	struct type_datum *source;
	struct type_datum *target;
	u32 masked = 0;

	source = flex_array_get_ptr(policydb.type_val_to_struct_array,
				    scontext->type - 1);
	BUG_ON(!source);

	target = flex_array_get_ptr(policydb.type_val_to_struct_array,
				    tcontext->type - 1);
	BUG_ON(!target);

	if (source->bounds) {
		memset(&lo_avd, 0, sizeof(lo_avd));

		memcpy(&lo_scontext, scontext, sizeof(lo_scontext));
		lo_scontext.type = source->bounds;

		context_struct_compute_av(&lo_scontext,
					  tcontext,
					  tclass,
					  &lo_avd);
		if ((lo_avd.allowed & avd->allowed) == avd->allowed)
			return;		/* no masked permission */
		masked = ~lo_avd.allowed & avd->allowed;
	}

	if (target->bounds) {
		memset(&lo_avd, 0, sizeof(lo_avd));

		memcpy(&lo_tcontext, tcontext, sizeof(lo_tcontext));
		lo_tcontext.type = target->bounds;

		context_struct_compute_av(scontext,
					  &lo_tcontext,
					  tclass,
					  &lo_avd);
		if ((lo_avd.allowed & avd->allowed) == avd->allowed)
			return;		/* no masked permission */
		masked = ~lo_avd.allowed & avd->allowed;
	}

	if (source->bounds && target->bounds) {
		memset(&lo_avd, 0, sizeof(lo_avd));
		/*
		 * lo_scontext and lo_tcontext are already
		 * set up.
		 */

		context_struct_compute_av(&lo_scontext,
					  &lo_tcontext,
					  tclass,
					  &lo_avd);
		if ((lo_avd.allowed & avd->allowed) == avd->allowed)
			return;		/* no masked permission */
		masked = ~lo_avd.allowed & avd->allowed;
	}

	if (masked) {
		/* mask violated permissions */
		avd->allowed &= ~masked;

		/* audit masked permissions */
		security_dump_masked_av(scontext, tcontext,
					tclass, masked, "bounds");
	}
}
