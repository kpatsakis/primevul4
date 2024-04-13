AcpiDsCreateOperand (
    ACPI_WALK_STATE         *WalkState,
    ACPI_PARSE_OBJECT       *Arg,
    UINT32                  ArgIndex)
{
    ACPI_STATUS             Status = AE_OK;
    char                    *NameString;
    UINT32                  NameLength;
    ACPI_OPERAND_OBJECT     *ObjDesc;
    ACPI_PARSE_OBJECT       *ParentOp;
    UINT16                  Opcode;
    ACPI_INTERPRETER_MODE   InterpreterMode;
    const ACPI_OPCODE_INFO  *OpInfo;


    ACPI_FUNCTION_TRACE_PTR (DsCreateOperand, Arg);


    /* A valid name must be looked up in the namespace */

    if ((Arg->Common.AmlOpcode == AML_INT_NAMEPATH_OP) &&
        (Arg->Common.Value.String) &&
        !(Arg->Common.Flags & ACPI_PARSEOP_IN_STACK))
    {
        ACPI_DEBUG_PRINT ((ACPI_DB_DISPATCH, "Getting a name: Arg=%p\n", Arg));

        /* Get the entire name string from the AML stream */

        Status = AcpiExGetNameString (ACPI_TYPE_ANY,
            Arg->Common.Value.Buffer, &NameString, &NameLength);

        if (ACPI_FAILURE (Status))
        {
            return_ACPI_STATUS (Status);
        }

        /* All prefixes have been handled, and the name is in NameString */

        /*
         * Special handling for BufferField declarations. This is a deferred
         * opcode that unfortunately defines the field name as the last
         * parameter instead of the first. We get here when we are performing
         * the deferred execution, so the actual name of the field is already
         * in the namespace. We don't want to attempt to look it up again
         * because we may be executing in a different scope than where the
         * actual opcode exists.
         */
        if ((WalkState->DeferredNode) &&
            (WalkState->DeferredNode->Type == ACPI_TYPE_BUFFER_FIELD) &&
            (ArgIndex == (UINT32)
                ((WalkState->Opcode == AML_CREATE_FIELD_OP) ? 3 : 2)))
        {
            ObjDesc = ACPI_CAST_PTR (
                ACPI_OPERAND_OBJECT, WalkState->DeferredNode);
            Status = AE_OK;
        }
        else    /* All other opcodes */
        {
            /*
             * Differentiate between a namespace "create" operation
             * versus a "lookup" operation (IMODE_LOAD_PASS2 vs.
             * IMODE_EXECUTE) in order to support the creation of
             * namespace objects during the execution of control methods.
             */
            ParentOp = Arg->Common.Parent;
            OpInfo = AcpiPsGetOpcodeInfo (ParentOp->Common.AmlOpcode);

            if ((OpInfo->Flags & AML_NSNODE) &&
                (ParentOp->Common.AmlOpcode != AML_INT_METHODCALL_OP) &&
                (ParentOp->Common.AmlOpcode != AML_REGION_OP) &&
                (ParentOp->Common.AmlOpcode != AML_INT_NAMEPATH_OP))
            {
                /* Enter name into namespace if not found */

                InterpreterMode = ACPI_IMODE_LOAD_PASS2;
            }
            else
            {
                /* Return a failure if name not found */

                InterpreterMode = ACPI_IMODE_EXECUTE;
            }

            Status = AcpiNsLookup (WalkState->ScopeInfo, NameString,
                ACPI_TYPE_ANY, InterpreterMode,
                ACPI_NS_SEARCH_PARENT | ACPI_NS_DONT_OPEN_SCOPE, WalkState,
                ACPI_CAST_INDIRECT_PTR (ACPI_NAMESPACE_NODE, &ObjDesc));
            /*
             * The only case where we pass through (ignore) a NOT_FOUND
             * error is for the CondRefOf opcode.
             */
            if (Status == AE_NOT_FOUND)
            {
                if (ParentOp->Common.AmlOpcode == AML_CONDITIONAL_REF_OF_OP)
                {
                    /*
                     * For the Conditional Reference op, it's OK if
                     * the name is not found;  We just need a way to
                     * indicate this to the interpreter, set the
                     * object to the root
                     */
                    ObjDesc = ACPI_CAST_PTR (
                        ACPI_OPERAND_OBJECT, AcpiGbl_RootNode);
                    Status = AE_OK;
                }
                else if (ParentOp->Common.AmlOpcode == AML_EXTERNAL_OP)
                {
                    /*
                     * This opcode should never appear here. It is used only
                     * by AML disassemblers and is surrounded by an If(0)
                     * by the ASL compiler.
                     *
                     * Therefore, if we see it here, it is a serious error.
                     */
                    Status = AE_AML_BAD_OPCODE;
                }
                else
                {
                    /*
                     * We just plain didn't find it -- which is a
                     * very serious error at this point
                     */
                    Status = AE_AML_NAME_NOT_FOUND;
                }
            }

            if (ACPI_FAILURE (Status))
            {
                ACPI_ERROR_NAMESPACE (NameString, Status);
            }
        }

        /* Free the namestring created above */

        ACPI_FREE (NameString);

        /* Check status from the lookup */

        if (ACPI_FAILURE (Status))
        {
            return_ACPI_STATUS (Status);
        }

        /* Put the resulting object onto the current object stack */

        Status = AcpiDsObjStackPush (ObjDesc, WalkState);
        if (ACPI_FAILURE (Status))
        {
            return_ACPI_STATUS (Status);
        }

        AcpiDbDisplayArgumentObject (ObjDesc, WalkState);
    }
    else
    {
        /* Check for null name case */

        if ((Arg->Common.AmlOpcode == AML_INT_NAMEPATH_OP) &&
            !(Arg->Common.Flags & ACPI_PARSEOP_IN_STACK))
        {
            /*
             * If the name is null, this means that this is an
             * optional result parameter that was not specified
             * in the original ASL. Create a Zero Constant for a
             * placeholder. (Store to a constant is a Noop.)
             */
            Opcode = AML_ZERO_OP;       /* Has no arguments! */

            ACPI_DEBUG_PRINT ((ACPI_DB_DISPATCH,
                "Null namepath: Arg=%p\n", Arg));
        }
        else
        {
            Opcode = Arg->Common.AmlOpcode;
        }

        /* Get the object type of the argument */

        OpInfo = AcpiPsGetOpcodeInfo (Opcode);
        if (OpInfo->ObjectType == ACPI_TYPE_INVALID)
        {
            return_ACPI_STATUS (AE_NOT_IMPLEMENTED);
        }

        if ((OpInfo->Flags & AML_HAS_RETVAL) ||
            (Arg->Common.Flags & ACPI_PARSEOP_IN_STACK))
        {
            /*
             * Use value that was already previously returned
             * by the evaluation of this argument
             */
            Status = AcpiDsResultPop (&ObjDesc, WalkState);
            if (ACPI_FAILURE (Status))
            {
                /*
                 * Only error is underflow, and this indicates
                 * a missing or null operand!
                 */
                ACPI_EXCEPTION ((AE_INFO, Status,
                    "Missing or null operand"));
                return_ACPI_STATUS (Status);
            }
        }
        else
        {
            /* Create an ACPI_INTERNAL_OBJECT for the argument */

            ObjDesc = AcpiUtCreateInternalObject (OpInfo->ObjectType);
            if (!ObjDesc)
            {
                return_ACPI_STATUS (AE_NO_MEMORY);
            }

            /* Initialize the new object */

            Status = AcpiDsInitObjectFromOp (
                WalkState, Arg, Opcode, &ObjDesc);
            if (ACPI_FAILURE (Status))
            {
                AcpiUtDeleteObjectDesc (ObjDesc);
                return_ACPI_STATUS (Status);
            }
        }

        /* Put the operand object on the object stack */

        Status = AcpiDsObjStackPush (ObjDesc, WalkState);
        if (ACPI_FAILURE (Status))
        {
            return_ACPI_STATUS (Status);
        }

        AcpiDbDisplayArgumentObject (ObjDesc, WalkState);
    }

    return_ACPI_STATUS (AE_OK);
}
