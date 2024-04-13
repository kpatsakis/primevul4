    QueueHandle_t xQueueCreateCountingSemaphore( const UBaseType_t uxMaxCount,
                                                 const UBaseType_t uxInitialCount )
    {
        QueueHandle_t xHandle;

        configASSERT( uxMaxCount != 0 );
        configASSERT( uxInitialCount <= uxMaxCount );

        xHandle = xQueueGenericCreate( uxMaxCount, queueSEMAPHORE_QUEUE_ITEM_LENGTH, queueQUEUE_TYPE_COUNTING_SEMAPHORE );

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