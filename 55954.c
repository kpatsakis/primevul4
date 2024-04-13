AcpiDsClearImplicitReturn (
    ACPI_WALK_STATE         *WalkState)
{
    ACPI_FUNCTION_NAME (DsClearImplicitReturn);


    /*
     * Slack must be enabled for this feature
     */
    if (!AcpiGbl_EnableInterpreterSlack)
    {
        return;
    }

    if (WalkState->ImplicitReturnObj)
    {
        /*
         * Delete any "stale" implicit return. However, in
         * complex statements, the implicit return value can be
         * bubbled up several levels.
         */
        ACPI_DEBUG_PRINT ((ACPI_DB_DISPATCH,
            "Removing reference on stale implicit return obj %p\n",
            WalkState->ImplicitReturnObj));

        AcpiUtRemoveReference (WalkState->ImplicitReturnObj);
        WalkState->ImplicitReturnObj = NULL;
    }
}
