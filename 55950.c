AcpiPsBuildNamedOp (
    ACPI_WALK_STATE         *WalkState,
    UINT8                   *AmlOpStart,
    ACPI_PARSE_OBJECT       *UnnamedOp,
    ACPI_PARSE_OBJECT       **Op)
{
    ACPI_STATUS             Status = AE_OK;
    ACPI_PARSE_OBJECT       *Arg = NULL;


    ACPI_FUNCTION_TRACE_PTR (PsBuildNamedOp, WalkState);


    UnnamedOp->Common.Value.Arg = NULL;
    UnnamedOp->Common.ArgListLength = 0;
    UnnamedOp->Common.AmlOpcode = WalkState->Opcode;

    /*
     * Get and append arguments until we find the node that contains
     * the name (the type ARGP_NAME).
     */
    while (GET_CURRENT_ARG_TYPE (WalkState->ArgTypes) &&
          (GET_CURRENT_ARG_TYPE (WalkState->ArgTypes) != ARGP_NAME))
    {
        ASL_CV_CAPTURE_COMMENTS (WalkState);
        Status = AcpiPsGetNextArg (WalkState, &(WalkState->ParserState),
            GET_CURRENT_ARG_TYPE (WalkState->ArgTypes), &Arg);
        if (ACPI_FAILURE (Status))
        {
            return_ACPI_STATUS (Status);
        }

        AcpiPsAppendArg (UnnamedOp, Arg);
        INCREMENT_ARG_LIST (WalkState->ArgTypes);
    }

    /* are there any inline comments associated with the NameSeg?? If so, save this. */

    ASL_CV_CAPTURE_COMMENTS (WalkState);

#ifdef ACPI_ASL_COMPILER
    if (AcpiGbl_CurrentInlineComment != NULL)
    {
        UnnamedOp->Common.NameComment = AcpiGbl_CurrentInlineComment;
        AcpiGbl_CurrentInlineComment = NULL;
    }
#endif

    /*
     * Make sure that we found a NAME and didn't run out of arguments
     */
    if (!GET_CURRENT_ARG_TYPE (WalkState->ArgTypes))
    {
        return_ACPI_STATUS (AE_AML_NO_OPERAND);
    }

    /* We know that this arg is a name, move to next arg */

    INCREMENT_ARG_LIST (WalkState->ArgTypes);

    /*
     * Find the object. This will either insert the object into
     * the namespace or simply look it up
     */
    WalkState->Op = NULL;

    Status = WalkState->DescendingCallback (WalkState, Op);
    if (ACPI_FAILURE (Status))
    {
        if (Status != AE_CTRL_TERMINATE)
        {
            ACPI_EXCEPTION ((AE_INFO, Status, "During name lookup/catalog"));
        }
        return_ACPI_STATUS (Status);
    }

    if (!*Op)
    {
        return_ACPI_STATUS (AE_CTRL_PARSE_CONTINUE);
    }

    Status = AcpiPsNextParseState (WalkState, *Op, Status);
    if (ACPI_FAILURE (Status))
    {
        if (Status == AE_CTRL_PENDING)
        {
            Status = AE_CTRL_PARSE_PENDING;
        }
        return_ACPI_STATUS (Status);
    }

    AcpiPsAppendArg (*Op, UnnamedOp->Common.Value.Arg);

#ifdef ACPI_ASL_COMPILER

    /* save any comments that might be associated with UnnamedOp. */

    (*Op)->Common.InlineComment     = UnnamedOp->Common.InlineComment;
    (*Op)->Common.EndNodeComment    = UnnamedOp->Common.EndNodeComment;
    (*Op)->Common.CloseBraceComment = UnnamedOp->Common.CloseBraceComment;
    (*Op)->Common.NameComment       = UnnamedOp->Common.NameComment;
    (*Op)->Common.CommentList       = UnnamedOp->Common.CommentList;
    (*Op)->Common.EndBlkComment     = UnnamedOp->Common.EndBlkComment;
    (*Op)->Common.CvFilename        = UnnamedOp->Common.CvFilename;
    (*Op)->Common.CvParentFilename  = UnnamedOp->Common.CvParentFilename;
    (*Op)->Named.Aml                = UnnamedOp->Common.Aml;

    UnnamedOp->Common.InlineComment     = NULL;
    UnnamedOp->Common.EndNodeComment    = NULL;
    UnnamedOp->Common.CloseBraceComment = NULL;
    UnnamedOp->Common.NameComment       = NULL;
    UnnamedOp->Common.CommentList       = NULL;
    UnnamedOp->Common.EndBlkComment     = NULL;
#endif

    if ((*Op)->Common.AmlOpcode == AML_REGION_OP ||
        (*Op)->Common.AmlOpcode == AML_DATA_REGION_OP)
    {
        /*
         * Defer final parsing of an OperationRegion body, because we don't
         * have enough info in the first pass to parse it correctly (i.e.,
         * there may be method calls within the TermArg elements of the body.)
         *
         * However, we must continue parsing because the opregion is not a
         * standalone package -- we don't know where the end is at this point.
         *
         * (Length is unknown until parse of the body complete)
         */
        (*Op)->Named.Data = AmlOpStart;
        (*Op)->Named.Length = 0;
    }

    return_ACPI_STATUS (AE_OK);
}
