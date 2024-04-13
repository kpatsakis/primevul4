    static BaseType_t prvNotifyQueueSetContainer( const Queue_t * const pxQueue )
    {
        Queue_t * pxQueueSetContainer = pxQueue->pxQueueSetContainer;
        BaseType_t xReturn = pdFALSE;

        /* This function must be called form a critical section. */

        configASSERT( pxQueueSetContainer );
        configASSERT( pxQueueSetContainer->uxMessagesWaiting < pxQueueSetContainer->uxLength );

        if( pxQueueSetContainer->uxMessagesWaiting < pxQueueSetContainer->uxLength )
        {
            const int8_t cTxLock = pxQueueSetContainer->cTxLock;

            traceQUEUE_SET_SEND( pxQueueSetContainer );

            /* The data copied is the handle of the queue that contains data. */
            xReturn = prvCopyDataToQueue( pxQueueSetContainer, &pxQueue, queueSEND_TO_BACK );

            if( cTxLock == queueUNLOCKED )
            {
                if( listLIST_IS_EMPTY( &( pxQueueSetContainer->xTasksWaitingToReceive ) ) == pdFALSE )
                {
                    if( xTaskRemoveFromEventList( &( pxQueueSetContainer->xTasksWaitingToReceive ) ) != pdFALSE )
                    {
                        /* The task waiting has a higher priority. */
                        xReturn = pdTRUE;
                    }
                    else
                    {
                        mtCOVERAGE_TEST_MARKER();
                    }
                }
                else
                {
                    mtCOVERAGE_TEST_MARKER();
                }
            }
            else
            {
                configASSERT( cTxLock != queueINT8_MAX );

                pxQueueSetContainer->cTxLock = ( int8_t ) ( cTxLock + 1 );
            }
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();
        }

        return xReturn;
    }