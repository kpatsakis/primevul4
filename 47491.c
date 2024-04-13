int main(int argc,char **argv){
  pid_t mypid=getpid();
  pthread_t counter_thread={0};
  int num_cpus=0;
  int *timerpids;
#if TIMERCHECKS
  int force=0;
#endif
  int testing=0;

  {
    while(gettimerpid(NULL,num_cpus)!=-1)
      num_cpus++;
    timerpids=malloc(sizeof(int)*num_cpus);

    {
      int cpu=0;
      for(cpu=0;cpu<num_cpus;cpu++)
	timerpids[cpu]=gettimerpid(NULL,cpu);
    }
  }


  OPTARGS_BEGIN("Usage: das_watchdog [--force] [--verbose] [--checkirq] [--increasetime n] [--checktime n] [--waittime n]\n"
		"                    [ -f]     [ -v]       [ -c]        [ -it n]           [ -ct n]        [ -wt n]\n"
		"\n"
		"Additional arguments:\n"
		"[--version] or [-ve]              -> Prints out version.\n"
		"[--test]    or [-te]              -> Run a test to see if xmessage is working.\n")
    
    {
      OPTARG("--verbose","-v") verbose=1;
#if TIMERCHECKS
      OPTARG("--force","-f") force=1;
      OPTARG("--checkirq","-c") checkirq=1; return(checksoftirq(0));
#endif
      OPTARG("--increasetime","-it") increasetime=OPTARG_GETINT();
      OPTARG("--checktime","-ct") checktime=OPTARG_GETINT();
      OPTARG("--waittime","-wt") waittime=OPTARG_GETINT();
      OPTARG("--test","-te") testing=1; verbose=1;
      OPTARG("--version","-ve") printf("Das Version die Uhr Hund %s nach sein bist.\n",VERSION);exit(0);
    }OPTARGS_END;
  

  {
    openlog("das_watchdog", 0, LOG_DAEMON);
    syslog(LOG_INFO, "started");
  }
  
  {
#if TIMERCHECKS    
    if(force && checksoftirq(force)<0)
      return -2;

    checksoftirq(force);
#endif

    if(getuid()!=0){
      print_error(stdout,"Warning, you are not running as root. das_watchdog should be run as an init-script at startup, and not as a normal user.\n");
    }
    
    
    if(access(WHICH_XMESSAGE,X_OK)!=0){
      print_error(stderr,"Warning, \"xmessage\" is not found or is not an executable. I will try to use the $PATH instead. Hopefully that'll work,");
      print_error(stderr,"but you might not receive messages to the screen in case das_watchdog has to take action.");
      xmessage_found=0;
    }
  }


  if(1)  {
    if( ! set_pid_priority(0,SCHED_FIFO,sched_get_priority_max(SCHED_FIFO),
			   "Unable to set SCHED_FIFO realtime priority for %d (\"%s\"). (%s). Exiting.",
			   "Der Gewinde nach die Uhr Hund"))
      return 0;
    if(mlockall(MCL_CURRENT|MCL_FUTURE)==-1)
      print_error(stderr,"Could not call mlockalll(MCL_CURRENT|MCL_FUTURE) (%s)",strerror(errno));
  }


  {
    pthread_create(&counter_thread,NULL,counter_func,NULL);
  }


  for(;;){
    int lastcounter=counter;

    sleep(checktime);
    if(verbose)
      print_error(stderr,"    counter read to be %d  (lastcounter=%d)",counter,lastcounter);
    
    if(lastcounter==counter || testing==1){
      int changedsched=0;
      struct proclistlist *pll=pll_create();
      int lokke;

      if(verbose)
	print_error(stdout,"Die Uhr Hund stossen sein!");

      for(lokke=0;lokke<pll->length;lokke++){
	if(pll->proclist[lokke].policy!=SCHED_OTHER 
	   && pll->proclist[lokke].pid!=mypid 
	   && (!is_a_member(pll->proclist[lokke].pid,timerpids,num_cpus))
	   )
	  {
	    struct sched_param par={0};
	    par.sched_priority=0;
	    if(verbose)
	      print_error(stdout,"Setting pid %d temporarily to SCHED_OTHER.",pll->proclist[lokke].pid);
	    if(set_pid_priority(pll->proclist[lokke].pid,SCHED_OTHER,0,"Could not set pid %d (\"%s\") to SCHED_OTHER (%s).\n","no name"))
	      changedsched++;
	  }
      }

      if(changedsched>0 || testing==1){

        print_error(NULL,"realtime operations paused for %d seconds.",waittime);

	if(fork()==0){
	  xmessage_fork(pll);
	  return 0;
	}

	sleep(waittime);

	for(lokke=0;lokke<pll->length;lokke++){
	  if(pll->proclist[lokke].policy != SCHED_OTHER 
	     && pll->proclist[lokke].pid != mypid 
	     && (!is_a_member(pll->proclist[lokke].pid,timerpids,num_cpus))
	     && pll->proclist[lokke].start_time == get_pid_start_time(pll->proclist[lokke].pid) 
	     )
	    {
	      if(get_pid_priority(pll->proclist[lokke].pid)      != 0
                 || sched_getscheduler(pll->proclist[lokke].pid) != SCHED_OTHER){
		print_error(stderr,
			    "Seems like someone else has changed priority and/or scheduling policy for %d in the mean time. I'm not going to do anything.",
			    pll->proclist[lokke].pid);
	      }else{
		struct sched_param par={0};
		par.sched_priority=pll->proclist[lokke].priority;
		if(verbose)
		  print_error(stdout,"Setting pid %d back to realtime priority.",pll->proclist[lokke].pid);
		set_pid_priority(pll->proclist[lokke].pid,pll->proclist[lokke].policy,pll->proclist[lokke].priority,"Could not set pid %d (\"%s\") to SCHED_FIFO/SCHED_RR (%s).\n\n", "no name");
	      }
	    }
	}
      }
      pll_delete(pll);
    }
    if(testing==1) break;
  }
  
  return 0;
}
