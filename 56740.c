u32 acpi_ns_opens_scope(acpi_object_type type)
{
	ACPI_FUNCTION_ENTRY();

	if (type > ACPI_TYPE_LOCAL_MAX) {

		/* type code out of range  */

		ACPI_WARNING((AE_INFO, "Invalid Object Type 0x%X", type));
		return (ACPI_NS_NORMAL);
	}

	return (((u32)acpi_gbl_ns_properties[type]) & ACPI_NS_NEWSCOPE);
}
