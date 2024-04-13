    QueueHandle_t xQueueGenericCreateStatic( const UBaseType_t uxQueueLength,
                                             const UBaseType_t uxItemSize,
                                             uint8_t * pucQueueStorage,
                                             StaticQueue_t * pxStaticQueue,
                                             const uint8_t ucQueueType )
    {
        Queue_t * pxNewQueue;

        configASSERT( uxQueueLength > ( UBaseType_t ) 0 );

        /* The StaticQueue_t structure and the queue storage area must be
         * supplied. */
        configASSERT( pxStaticQueue != NULL );

        /* A queue storage area should be provided if the item size is not 0, and
         * should not be provided if the item size is 0. */
        configASSERT( !( ( pucQueueStorage != NULL ) && ( uxItemSize == 0 ) ) );
        configASSERT( !( ( pucQueueStorage == NULL ) && ( uxItemSize != 0 ) ) );

        #if ( configASSERT_DEFINED == 1 )
            {
                /* Sanity check that the size of the structure used to declare a
                 * variable of type StaticQueue_t or StaticSemaphore_t equals the size of
                 * the real queue and semaphore structures. */
                volatile size_t xSize = sizeof( StaticQueue_t );
                configASSERT( xSize == sizeof( Queue_t ) );
                ( void ) xSize; /* Keeps lint quiet when configASSERT() is not defined. */
            }
        #endif /* configASSERT_DEFINED */

        /* The address of a statically allocated queue was passed in, use it.
         * The address of a statically allocated storage area was also passed in
         * but is already set. */
        pxNewQueue = ( Queue_t * ) pxStaticQueue; /*lint !e740 !e9087 Unusual cast is ok as the structures are designed to have the same alignment, and the size is checked by an assert. */

        if( pxNewQueue != NULL )
        {
            #if ( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
                {
                    /* Queues can be allocated wither statically or dynamically, so
                     * note this queue was allocated statically in case the queue is
                     * later deleted. */
                    pxNewQueue->ucStaticallyAllocated = pdTRUE;
                }
            #endif /* configSUPPORT_DYNAMIC_ALLOCATION */

            prvInitialiseNewQueue( uxQueueLength, uxItemSize, pucQueueStorage, ucQueueType, pxNewQueue );
        }
        else
        {
            traceQUEUE_CREATE_FAILED( ucQueueType );
            mtCOVERAGE_TEST_MARKER();
        }

        return pxNewQueue;
    }