AcpiDsEvaluateNamePath (
    ACPI_WALK_STATE         *WalkState)
{
    ACPI_STATUS             Status = AE_OK;
    ACPI_PARSE_OBJECT       *Op = WalkState->Op;
    ACPI_OPERAND_OBJECT     **Operand = &WalkState->Operands[0];
    ACPI_OPERAND_OBJECT     *NewObjDesc;
    UINT8                   Type;


    ACPI_FUNCTION_TRACE_PTR (DsEvaluateNamePath, WalkState);


    if (!Op->Common.Parent)
    {
        /* This happens after certain exception processing */

        goto Exit;
    }

    if ((Op->Common.Parent->Common.AmlOpcode == AML_PACKAGE_OP) ||
        (Op->Common.Parent->Common.AmlOpcode == AML_VARIABLE_PACKAGE_OP) ||
        (Op->Common.Parent->Common.AmlOpcode == AML_REF_OF_OP))
    {
        /* TBD: Should we specify this feature as a bit of OpInfo->Flags of these opcodes? */

        goto Exit;
    }

    Status = AcpiDsCreateOperand (WalkState, Op, 0);
    if (ACPI_FAILURE (Status))
    {
        goto Exit;
    }

    if (Op->Common.Flags & ACPI_PARSEOP_TARGET)
    {
        NewObjDesc = *Operand;
        goto PushResult;
    }

    Type = (*Operand)->Common.Type;

    Status = AcpiExResolveToValue (Operand, WalkState);
    if (ACPI_FAILURE (Status))
    {
        goto Exit;
    }

    if (Type == ACPI_TYPE_INTEGER)
    {
        /* It was incremented by AcpiExResolveToValue */

        AcpiUtRemoveReference (*Operand);

        Status = AcpiUtCopyIobjectToIobject (
            *Operand, &NewObjDesc, WalkState);
        if (ACPI_FAILURE (Status))
        {
            goto Exit;
        }
    }
    else
    {
        /*
         * The object either was anew created or is
         * a Namespace node - don't decrement it.
         */
        NewObjDesc = *Operand;
    }

    /* Cleanup for name-path operand */

    Status = AcpiDsObjStackPop (1, WalkState);
    if (ACPI_FAILURE (Status))
    {
        WalkState->ResultObj = NewObjDesc;
        goto Exit;
    }

PushResult:

    WalkState->ResultObj = NewObjDesc;

    Status = AcpiDsResultPush (WalkState->ResultObj, WalkState);
    if (ACPI_SUCCESS (Status))
    {
        /* Force to take it from stack */

        Op->Common.Flags |= ACPI_PARSEOP_IN_STACK;
    }

Exit:

    return_ACPI_STATUS (Status);
}
