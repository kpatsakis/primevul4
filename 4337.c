    BaseType_t xQueueTakeMutexRecursive( QueueHandle_t xMutex,
                                         TickType_t xTicksToWait )
    {
        BaseType_t xReturn;
        Queue_t * const pxMutex = ( Queue_t * ) xMutex;

        configASSERT( pxMutex );

        /* Comments regarding mutual exclusion as per those within
         * xQueueGiveMutexRecursive(). */

        traceTAKE_MUTEX_RECURSIVE( pxMutex );

        if( pxMutex->u.xSemaphore.xMutexHolder == xTaskGetCurrentTaskHandle() )
        {
            ( pxMutex->u.xSemaphore.uxRecursiveCallCount )++;
            xReturn = pdPASS;
        }
        else
        {
            xReturn = xQueueSemaphoreTake( pxMutex, xTicksToWait );

            /* pdPASS will only be returned if the mutex was successfully
             * obtained.  The calling task may have entered the Blocked state
             * before reaching here. */
            if( xReturn != pdFAIL )
            {
                ( pxMutex->u.xSemaphore.uxRecursiveCallCount )++;
            }
            else
            {
                traceTAKE_MUTEX_RECURSIVE_FAILED( pxMutex );
            }
        }

        return xReturn;
    }