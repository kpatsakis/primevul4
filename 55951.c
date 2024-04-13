AcpiPsCompleteOp (
    ACPI_WALK_STATE         *WalkState,
    ACPI_PARSE_OBJECT       **Op,
    ACPI_STATUS             Status)
{
    ACPI_STATUS             Status2;


    ACPI_FUNCTION_TRACE_PTR (PsCompleteOp, WalkState);


    /*
     * Finished one argument of the containing scope
     */
    WalkState->ParserState.Scope->ParseScope.ArgCount--;

    /* Close this Op (will result in parse subtree deletion) */

    Status2 = AcpiPsCompleteThisOp (WalkState, *Op);
    if (ACPI_FAILURE (Status2))
    {
        return_ACPI_STATUS (Status2);
    }

    *Op = NULL;

    switch (Status)
    {
    case AE_OK:

        break;

    case AE_CTRL_TRANSFER:

        /* We are about to transfer to a called method */

        WalkState->PrevOp = NULL;
        WalkState->PrevArgTypes = WalkState->ArgTypes;
        return_ACPI_STATUS (Status);

    case AE_CTRL_END:

        AcpiPsPopScope (&(WalkState->ParserState), Op,
            &WalkState->ArgTypes, &WalkState->ArgCount);

        if (*Op)
        {
            WalkState->Op = *Op;
            WalkState->OpInfo = AcpiPsGetOpcodeInfo ((*Op)->Common.AmlOpcode);
            WalkState->Opcode = (*Op)->Common.AmlOpcode;

            Status = WalkState->AscendingCallback (WalkState);
            Status = AcpiPsNextParseState (WalkState, *Op, Status);

            Status2 = AcpiPsCompleteThisOp (WalkState, *Op);
            if (ACPI_FAILURE (Status2))
            {
                return_ACPI_STATUS (Status2);
            }
        }

        Status = AE_OK;
        break;

    case AE_CTRL_BREAK:
    case AE_CTRL_CONTINUE:

        /* Pop off scopes until we find the While */

        while (!(*Op) || ((*Op)->Common.AmlOpcode != AML_WHILE_OP))
        {
            AcpiPsPopScope (&(WalkState->ParserState), Op,
                &WalkState->ArgTypes, &WalkState->ArgCount);
        }

        /* Close this iteration of the While loop */

        WalkState->Op = *Op;
        WalkState->OpInfo = AcpiPsGetOpcodeInfo ((*Op)->Common.AmlOpcode);
        WalkState->Opcode = (*Op)->Common.AmlOpcode;

        Status = WalkState->AscendingCallback (WalkState);
        Status = AcpiPsNextParseState (WalkState, *Op, Status);

        Status2 = AcpiPsCompleteThisOp (WalkState, *Op);
        if (ACPI_FAILURE (Status2))
        {
            return_ACPI_STATUS (Status2);
        }

        Status = AE_OK;
        break;

    case AE_CTRL_TERMINATE:

        /* Clean up */
        do
        {
            if (*Op)
            {
                Status2 = AcpiPsCompleteThisOp (WalkState, *Op);
                if (ACPI_FAILURE (Status2))
                {
                    return_ACPI_STATUS (Status2);
                }

                AcpiUtDeleteGenericState (
                    AcpiUtPopGenericState (&WalkState->ControlState));
            }

            AcpiPsPopScope (&(WalkState->ParserState), Op,
                &WalkState->ArgTypes, &WalkState->ArgCount);

        } while (*Op);

        return_ACPI_STATUS (AE_OK);

    default:  /* All other non-AE_OK status */

        do
        {
            if (*Op)
            {
                Status2 = AcpiPsCompleteThisOp (WalkState, *Op);
                if (ACPI_FAILURE (Status2))
                {
                    return_ACPI_STATUS (Status2);
                }
            }

            AcpiPsPopScope (&(WalkState->ParserState), Op,
                &WalkState->ArgTypes, &WalkState->ArgCount);

        } while (*Op);


#if 0
        /*
         * TBD: Cleanup parse ops on error
         */
        if (*Op == NULL)
        {
            AcpiPsPopScope (ParserState, Op,
                &WalkState->ArgTypes, &WalkState->ArgCount);
        }
#endif
        WalkState->PrevOp = NULL;
        WalkState->PrevArgTypes = WalkState->ArgTypes;
        return_ACPI_STATUS (Status);
    }

    /* This scope complete? */

    if (AcpiPsHasCompletedScope (&(WalkState->ParserState)))
    {
        AcpiPsPopScope (&(WalkState->ParserState), Op,
            &WalkState->ArgTypes, &WalkState->ArgCount);
        ACPI_DEBUG_PRINT ((ACPI_DB_PARSE, "Popped scope, Op=%p\n", *Op));
    }
    else
    {
        *Op = NULL;
    }

    return_ACPI_STATUS (AE_OK);
}
