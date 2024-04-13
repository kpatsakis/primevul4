AcpiNsExecModuleCode (
    ACPI_OPERAND_OBJECT     *MethodObj,
    ACPI_EVALUATE_INFO      *Info)
{
    ACPI_OPERAND_OBJECT     *ParentObj;
    ACPI_NAMESPACE_NODE     *ParentNode;
    ACPI_OBJECT_TYPE        Type;
    ACPI_STATUS             Status;


    ACPI_FUNCTION_TRACE (NsExecModuleCode);


    /*
     * Get the parent node. We cheat by using the NextObject field
     * of the method object descriptor.
     */
    ParentNode = ACPI_CAST_PTR (
        ACPI_NAMESPACE_NODE, MethodObj->Method.NextObject);
    Type = AcpiNsGetType (ParentNode);

    /*
     * Get the region handler and save it in the method object. We may need
     * this if an operation region declaration causes a _REG method to be run.
     *
     * We can't do this in AcpiPsLinkModuleCode because
     * AcpiGbl_RootNode->Object is NULL at PASS1.
     */
    if ((Type == ACPI_TYPE_DEVICE) && ParentNode->Object)
    {
        MethodObj->Method.Dispatch.Handler =
            ParentNode->Object->Device.Handler;
    }

    /* Must clear NextObject (AcpiNsAttachObject needs the field) */

    MethodObj->Method.NextObject = NULL;

    /* Initialize the evaluation information block */

    memset (Info, 0, sizeof (ACPI_EVALUATE_INFO));
    Info->PrefixNode = ParentNode;

    /*
     * Get the currently attached parent object. Add a reference,
     * because the ref count will be decreased when the method object
     * is installed to the parent node.
     */
    ParentObj = AcpiNsGetAttachedObject (ParentNode);
    if (ParentObj)
    {
        AcpiUtAddReference (ParentObj);
    }

    /* Install the method (module-level code) in the parent node */

    Status = AcpiNsAttachObject (ParentNode, MethodObj, ACPI_TYPE_METHOD);
    if (ACPI_FAILURE (Status))
    {
        goto Exit;
    }

    /* Execute the parent node as a control method */

    Status = AcpiNsEvaluate (Info);

    ACPI_DEBUG_PRINT ((ACPI_DB_INIT_NAMES,
        "Executed module-level code at %p\n",
        MethodObj->Method.AmlStart));

    /* Delete a possible implicit return value (in slack mode) */

    if (Info->ReturnObject)
    {
        AcpiUtRemoveReference (Info->ReturnObject);
    }

    /* Detach the temporary method object */

    AcpiNsDetachObject (ParentNode);

    /* Restore the original parent object */

    if (ParentObj)
    {
        Status = AcpiNsAttachObject (ParentNode, ParentObj, Type);
    }
    else
    {
        ParentNode->Type = (UINT8) Type;
    }

Exit:
    if (ParentObj)
    {
        AcpiUtRemoveReference (ParentObj);
    }
    return_VOID;
}
