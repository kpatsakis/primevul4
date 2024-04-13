    void vQueueAddToRegistry( QueueHandle_t xQueue,
                              const char * pcQueueName ) /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
    {
        UBaseType_t ux;

        /* See if there is an empty space in the registry.  A NULL name denotes
         * a free slot. */
        for( ux = ( UBaseType_t ) 0U; ux < ( UBaseType_t ) configQUEUE_REGISTRY_SIZE; ux++ )
        {
            if( xQueueRegistry[ ux ].pcQueueName == NULL )
            {
                /* Store the information on this queue. */
                xQueueRegistry[ ux ].pcQueueName = pcQueueName;
                xQueueRegistry[ ux ].xHandle = xQueue;

                traceQUEUE_REGISTRY_ADD( xQueue, pcQueueName );
                break;
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }
        }
    }