AcpiDsDeleteResultIfNotUsed (
    ACPI_PARSE_OBJECT       *Op,
    ACPI_OPERAND_OBJECT     *ResultObj,
    ACPI_WALK_STATE         *WalkState)
{
    ACPI_OPERAND_OBJECT     *ObjDesc;
    ACPI_STATUS             Status;


    ACPI_FUNCTION_TRACE_PTR (DsDeleteResultIfNotUsed, ResultObj);


    if (!Op)
    {
        ACPI_ERROR ((AE_INFO, "Null Op"));
        return_VOID;
    }

    if (!ResultObj)
    {
        return_VOID;
    }

    if (!AcpiDsIsResultUsed (Op, WalkState))
    {
        /* Must pop the result stack (ObjDesc should be equal to ResultObj) */

        Status = AcpiDsResultPop (&ObjDesc, WalkState);
        if (ACPI_SUCCESS (Status))
        {
            AcpiUtRemoveReference (ResultObj);
        }
    }

    return_VOID;
}
