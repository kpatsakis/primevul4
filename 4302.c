    const char * pcQueueGetName( QueueHandle_t xQueue ) /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
    {
        UBaseType_t ux;
        const char * pcReturn = NULL; /*lint !e971 Unqualified char types are allowed for strings and single characters only. */

        /* Note there is nothing here to protect against another task adding or
         * removing entries from the registry while it is being searched. */

        for( ux = ( UBaseType_t ) 0U; ux < ( UBaseType_t ) configQUEUE_REGISTRY_SIZE; ux++ )
        {
            if( xQueueRegistry[ ux ].xHandle == xQueue )
            {
                pcReturn = xQueueRegistry[ ux ].pcQueueName;
                break;
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }
        }

        return pcReturn;
    } /*lint !e818 xQueue cannot be a pointer to const because it is a typedef. */