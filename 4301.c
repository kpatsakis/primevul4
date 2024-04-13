    BaseType_t xQueueRemoveFromSet( QueueSetMemberHandle_t xQueueOrSemaphore,
                                    QueueSetHandle_t xQueueSet )
    {
        BaseType_t xReturn;
        Queue_t * const pxQueueOrSemaphore = ( Queue_t * ) xQueueOrSemaphore;

        if( pxQueueOrSemaphore->pxQueueSetContainer != xQueueSet )
        {
            /* The queue was not a member of the set. */
            xReturn = pdFAIL;
        }
        else if( pxQueueOrSemaphore->uxMessagesWaiting != ( UBaseType_t ) 0 )
        {
            /* It is dangerous to remove a queue from a set when the queue is
             * not empty because the queue set will still hold pending events for
             * the queue. */
            xReturn = pdFAIL;
        }
        else
        {
            taskENTER_CRITICAL();
            {
                /* The queue is no longer contained in the set. */
                pxQueueOrSemaphore->pxQueueSetContainer = NULL;
            }
            taskEXIT_CRITICAL();
            xReturn = pdPASS;
        }

        return xReturn;
    } /*lint !e818 xQueueSet could not be declared as pointing to const as it is a typedef. */