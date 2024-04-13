static void map_decision(u16 tclass, struct av_decision *avd,
			 int allow_unknown)
{
	if (tclass < current_mapping_size) {
		unsigned i, n = current_mapping[tclass].num_perms;
		u32 result;

		for (i = 0, result = 0; i < n; i++) {
			if (avd->allowed & current_mapping[tclass].perms[i])
				result |= 1<<i;
			if (allow_unknown && !current_mapping[tclass].perms[i])
				result |= 1<<i;
		}
		avd->allowed = result;

		for (i = 0, result = 0; i < n; i++)
			if (avd->auditallow & current_mapping[tclass].perms[i])
				result |= 1<<i;
		avd->auditallow = result;

		for (i = 0, result = 0; i < n; i++) {
			if (avd->auditdeny & current_mapping[tclass].perms[i])
				result |= 1<<i;
			if (!allow_unknown && !current_mapping[tclass].perms[i])
				result |= 1<<i;
		}
		/*
		 * In case the kernel has a bug and requests a permission
		 * between num_perms and the maximum permission number, we
		 * should audit that denial
		 */
		for (; i < (sizeof(u32)*8); i++)
			result |= 1<<i;
		avd->auditdeny = result;
	}
}
