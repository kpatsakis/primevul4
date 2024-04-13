    QueueHandle_t xQueueCreateCountingSemaphoreStatic( const UBaseType_t uxMaxCount,
                                                       const UBaseType_t uxInitialCount,
                                                       StaticQueue_t * pxStaticQueue )
    {
        QueueHandle_t xHandle;

        configASSERT( uxMaxCount != 0 );
        configASSERT( uxInitialCount <= uxMaxCount );

        xHandle = xQueueGenericCreateStatic( uxMaxCount, queueSEMAPHORE_QUEUE_ITEM_LENGTH, NULL, pxStaticQueue, queueQUEUE_TYPE_COUNTING_SEMAPHORE );

        if( xHandle != NULL )
        {
            ( ( Queue_t * ) xHandle )->uxMessagesWaiting = uxInitialCount;

            traceCREATE_COUNTING_SEMAPHORE();
        }
        else
        {
            traceCREATE_COUNTING_SEMAPHORE_FAILED();
        }

        return xHandle;
    }