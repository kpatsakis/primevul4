acpi_ns_internalize_name(const char *external_name, char **converted_name)
{
	char *internal_name;
	struct acpi_namestring_info info;
	acpi_status status;

	ACPI_FUNCTION_TRACE(ns_internalize_name);

	if ((!external_name) || (*external_name == 0) || (!converted_name)) {
		return_ACPI_STATUS(AE_BAD_PARAMETER);
	}

	/* Get the length of the new internal name */

	info.external_name = external_name;
	acpi_ns_get_internal_name_length(&info);

	/* We need a segment to store the internal  name */

	internal_name = ACPI_ALLOCATE_ZEROED(info.length);
	if (!internal_name) {
		return_ACPI_STATUS(AE_NO_MEMORY);
	}

	/* Build the name */

	info.internal_name = internal_name;
	status = acpi_ns_build_internal_name(&info);
	if (ACPI_FAILURE(status)) {
		ACPI_FREE(internal_name);
		return_ACPI_STATUS(status);
	}

	*converted_name = internal_name;
	return_ACPI_STATUS(AE_OK);
}
