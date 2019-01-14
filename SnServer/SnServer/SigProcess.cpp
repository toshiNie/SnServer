#include"stdafx.h"
#include"AsyncLog.h"
#include"Global.h"
#include"SigProcess.h"

void processCtrl_C(int sig)
{
	printf("ctrl_C");
	AsyncLog::getInstance().flush();
	Global::cancleFlag = true;
	printf("exit\n");
	exit(0);
	//kill(getpid(), 9);
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

Sigfunc* addSig(int sig, Sigfunc* func)
{
	return addSignal(sig, func);
}

Sigfunc* addSignal(int signo, Sigfunc* func)
{
	struct sigaction act, oact;
	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (signo == SIGALRM)
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
	if (ret < 0)
	{
		return nullptr;
	}
	return oact.sa_handler;
}
