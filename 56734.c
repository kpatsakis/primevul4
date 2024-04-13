acpi_ns_externalize_name(u32 internal_name_length,
			 const char *internal_name,
			 u32 * converted_name_length, char **converted_name)
{
	u32 names_index = 0;
	u32 num_segments = 0;
	u32 required_length;
	u32 prefix_length = 0;
	u32 i = 0;
	u32 j = 0;

	ACPI_FUNCTION_TRACE(ns_externalize_name);

	if (!internal_name_length || !internal_name || !converted_name) {
		return_ACPI_STATUS(AE_BAD_PARAMETER);
	}

	/* Check for a prefix (one '\' | one or more '^') */

	switch (internal_name[0]) {
	case AML_ROOT_PREFIX:

		prefix_length = 1;
		break;

	case AML_PARENT_PREFIX:

		for (i = 0; i < internal_name_length; i++) {
			if (ACPI_IS_PARENT_PREFIX(internal_name[i])) {
				prefix_length = i + 1;
			} else {
				break;
			}
		}

		if (i == internal_name_length) {
			prefix_length = i;
		}

		break;

	default:

		break;
	}

	/*
	 * Check for object names. Note that there could be 0-255 of these
	 * 4-byte elements.
	 */
	if (prefix_length < internal_name_length) {
		switch (internal_name[prefix_length]) {
		case AML_MULTI_NAME_PREFIX_OP:

			/* <count> 4-byte names */

			names_index = prefix_length + 2;
			num_segments = (u8)
			    internal_name[(acpi_size)prefix_length + 1];
			break;

		case AML_DUAL_NAME_PREFIX:

			/* Two 4-byte names */

			names_index = prefix_length + 1;
			num_segments = 2;
			break;

		case 0:

			/* null_name */

			names_index = 0;
			num_segments = 0;
			break;

		default:

			/* one 4-byte name */

			names_index = prefix_length;
			num_segments = 1;
			break;
		}
	}

	/*
	 * Calculate the length of converted_name, which equals the length
	 * of the prefix, length of all object names, length of any required
	 * punctuation ('.') between object names, plus the NULL terminator.
	 */
	required_length = prefix_length + (4 * num_segments) +
	    ((num_segments > 0) ? (num_segments - 1) : 0) + 1;

	/*
	 * Check to see if we're still in bounds. If not, there's a problem
	 * with internal_name (invalid format).
	 */
	if (required_length > internal_name_length) {
		ACPI_ERROR((AE_INFO, "Invalid internal name"));
		return_ACPI_STATUS(AE_BAD_PATHNAME);
	}

	/* Build the converted_name */

	*converted_name = ACPI_ALLOCATE_ZEROED(required_length);
	if (!(*converted_name)) {
		return_ACPI_STATUS(AE_NO_MEMORY);
	}

	j = 0;

	for (i = 0; i < prefix_length; i++) {
		(*converted_name)[j++] = internal_name[i];
	}

	if (num_segments > 0) {
		for (i = 0; i < num_segments; i++) {
			if (i > 0) {
				(*converted_name)[j++] = '.';
			}

			/* Copy and validate the 4-char name segment */

			ACPI_MOVE_NAME(&(*converted_name)[j],
				       &internal_name[names_index]);
			acpi_ut_repair_name(&(*converted_name)[j]);

			j += ACPI_NAME_SIZE;
			names_index += ACPI_NAME_SIZE;
		}
	}

	if (converted_name_length) {
		*converted_name_length = (u32) required_length;
	}

	return_ACPI_STATUS(AE_OK);
}
