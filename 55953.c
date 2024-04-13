AcpiPsGetAmlOpcode (
    ACPI_WALK_STATE         *WalkState)
{
    UINT32                  AmlOffset;


    ACPI_FUNCTION_TRACE_PTR (PsGetAmlOpcode, WalkState);


    WalkState->Aml = WalkState->ParserState.Aml;
    WalkState->Opcode = AcpiPsPeekOpcode (&(WalkState->ParserState));

    /*
     * First cut to determine what we have found:
     * 1) A valid AML opcode
     * 2) A name string
     * 3) An unknown/invalid opcode
     */
    WalkState->OpInfo = AcpiPsGetOpcodeInfo (WalkState->Opcode);

    switch (WalkState->OpInfo->Class)
    {
    case AML_CLASS_ASCII:
    case AML_CLASS_PREFIX:
        /*
         * Starts with a valid prefix or ASCII char, this is a name
         * string. Convert the bare name string to a namepath.
         */
        WalkState->Opcode = AML_INT_NAMEPATH_OP;
        WalkState->ArgTypes = ARGP_NAMESTRING;
        break;

    case AML_CLASS_UNKNOWN:

        /* The opcode is unrecognized. Complain and skip unknown opcodes */

        if (WalkState->PassNumber == 2)
        {
            AmlOffset = (UINT32) ACPI_PTR_DIFF (WalkState->Aml,
                WalkState->ParserState.AmlStart);

            ACPI_ERROR ((AE_INFO,
                "Unknown opcode 0x%.2X at table offset 0x%.4X, ignoring",
                WalkState->Opcode,
                (UINT32) (AmlOffset + sizeof (ACPI_TABLE_HEADER))));

            ACPI_DUMP_BUFFER ((WalkState->ParserState.Aml - 16), 48);

#ifdef ACPI_ASL_COMPILER
            /*
             * This is executed for the disassembler only. Output goes
             * to the disassembled ASL output file.
             */
            AcpiOsPrintf (
                "/*\nError: Unknown opcode 0x%.2X at table offset 0x%.4X, context:\n",
                WalkState->Opcode,
                (UINT32) (AmlOffset + sizeof (ACPI_TABLE_HEADER)));

            ACPI_ERROR ((AE_INFO,
                "Aborting disassembly, AML byte code is corrupt"));

            /* Dump the context surrounding the invalid opcode */

            AcpiUtDumpBuffer (((UINT8 *) WalkState->ParserState.Aml - 16),
                48, DB_BYTE_DISPLAY,
                (AmlOffset + sizeof (ACPI_TABLE_HEADER) - 16));
            AcpiOsPrintf (" */\n");

            /*
             * Just abort the disassembly, cannot continue because the
             * parser is essentially lost. The disassembler can then
             * randomly fail because an ill-constructed parse tree
             * can result.
             */
            return_ACPI_STATUS (AE_AML_BAD_OPCODE);
#endif
        }

        /* Increment past one-byte or two-byte opcode */

        WalkState->ParserState.Aml++;
        if (WalkState->Opcode > 0xFF) /* Can only happen if first byte is 0x5B */
        {
            WalkState->ParserState.Aml++;
        }

        return_ACPI_STATUS (AE_CTRL_PARSE_CONTINUE);

    default:

        /* Found opcode info, this is a normal opcode */

        WalkState->ParserState.Aml +=
            AcpiPsGetOpcodeSize (WalkState->Opcode);
        WalkState->ArgTypes = WalkState->OpInfo->ParseArgs;
        break;
    }

    return_ACPI_STATUS (AE_OK);
}
