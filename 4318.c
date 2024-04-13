    static void prvInitialiseMutex( Queue_t * pxNewQueue )
    {
        if( pxNewQueue != NULL )
        {
            /* The queue create function will set all the queue structure members
            * correctly for a generic queue, but this function is creating a
            * mutex.  Overwrite those members that need to be set differently -
            * in particular the information required for priority inheritance. */
            pxNewQueue->u.xSemaphore.xMutexHolder = NULL;
            pxNewQueue->uxQueueType = queueQUEUE_IS_MUTEX;

            /* In case this is a recursive mutex. */
            pxNewQueue->u.xSemaphore.uxRecursiveCallCount = 0;

            traceCREATE_MUTEX( pxNewQueue );

            /* Start with the semaphore in the expected state. */
            ( void ) xQueueGenericSend( pxNewQueue, NULL, ( TickType_t ) 0U, queueSEND_TO_BACK );
        }
        else
        {
            traceCREATE_MUTEX_FAILED();
        }
    }