#include"stdafx.h"
#include"LogThread.h"
#include"SigProcess.h"

void processCtrl_C(int sig)
{
	printf("ctrl_C:%d\n",errno);
	LogThread::getInstance().Flush();
	exit(0);
}


void registeSig()
{
	std::cout << "registeSig" << std::endl;
	Sigfunc * func = addSig(SIGINT, processCtrl_C);
}

void blockSIGINT(int signo) {
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	sigprocmask(SIG_BLOCK, &set, NULL);
}

Sigfunc*  addSig(int sig, Sigfunc* func)
{
	return signal(sig, func);
}

Sigfunc* addSignal(int signo, Sigfunc* func)
{
	struct sigaction act, oact;
	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (signo = SIGALRM)
	{
#ifdef SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT;
#endif
	}
	else
	{
#ifdef SA_RESTART
		act.sa_flags |= SA_RESTART;
#endif
	}
	int ret = sigaction(signo, &act, &oact);
	std::cout << ret << std::endl;
	if (ret < 0)
	{
		return NULL;
	}
	return oact.sa_handler;
}
