bool sched_can_stop_tick(void)
{
       struct rq *rq;

       rq = this_rq();

       /* Make sure rq->nr_running update is visible after the IPI */
       smp_rmb();

       /* More than one running task need preemption */
       if (rq->nr_running > 1)
               return false;

       return true;
}
