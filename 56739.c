u32 acpi_ns_local(acpi_object_type type)
{
	ACPI_FUNCTION_TRACE(ns_local);

	if (!acpi_ut_valid_object_type(type)) {

		/* Type code out of range  */

		ACPI_WARNING((AE_INFO, "Invalid Object Type 0x%X", type));
		return_UINT32(ACPI_NS_NORMAL);
	}

	return_UINT32(acpi_gbl_ns_properties[type] & ACPI_NS_LOCAL);
}
