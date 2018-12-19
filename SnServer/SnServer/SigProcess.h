#pragma once
#ifndef  SIGPROCESS_H_
#define  SIGPROCESS_H_
#include <signal.h>
#include <stdlib.h>
typedef void Sigfunc(int);
void processCtrl_C(int sig);
void registeSig();
void blockSIGINT(int signo);
Sigfunc* addSig(int sig, Sigfunc* func);
Sigfunc* addSignal(int signo, Sigfunc* func);

#endif // ! SIGPROCESS_H_
