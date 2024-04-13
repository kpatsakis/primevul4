AcpiNsExecModuleCodeList (
    void)
{
    ACPI_OPERAND_OBJECT     *Prev;
    ACPI_OPERAND_OBJECT     *Next;
    ACPI_EVALUATE_INFO      *Info;
    UINT32                  MethodCount = 0;


    ACPI_FUNCTION_TRACE (NsExecModuleCodeList);


    /* Exit now if the list is empty */

    Next = AcpiGbl_ModuleCodeList;
    if (!Next)
    {
        return_VOID;
    }

    /* Allocate the evaluation information block */

    Info = ACPI_ALLOCATE (sizeof (ACPI_EVALUATE_INFO));
    if (!Info)
    {
        return_VOID;
    }

    /* Walk the list, executing each "method" */

    while (Next)
    {
        Prev = Next;
        Next = Next->Method.Mutex;

        /* Clear the link field and execute the method */

        Prev->Method.Mutex = NULL;
        AcpiNsExecModuleCode (Prev, Info);
        MethodCount++;

        /* Delete the (temporary) method object */

        AcpiUtRemoveReference (Prev);
    }

    ACPI_INFO ((
        "Executed %u blocks of module-level executable AML code",
        MethodCount));

    ACPI_FREE (Info);
    AcpiGbl_ModuleCodeList = NULL;
    return_VOID;
}
