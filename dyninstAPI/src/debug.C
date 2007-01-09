/*
 * Copyright (c) 1996-2004 Barton P. Miller
 * 
 * We provide the Paradyn Parallel Performance Tools (below
 * described as "Paradyn") on an AS IS basis, and do not warrant its
 * validity or performance.  We reserve the right to update, modify,
 * or discontinue this software at any time.  We shall have no
 * obligation to supply such updates or modifications or any other
 * form of support to you.
 * 
 * This license is for research uses.  For such uses, there is no
 * charge. We define "research use" to mean you may freely use it
 * inside your organization for whatever purposes you see fit. But you
 * may not re-distribute Paradyn or parts of Paradyn, in any form
 * source or binary (including derivatives), electronic or otherwise,
 * to any other organization or entity without our permission.
 * 
 * (for other uses, please contact us at paradyn@cs.wisc.edu)
 * 
 * All warranties, including without limitation, any warranty of
 * merchantability or fitness for a particular purpose, are hereby
 * excluded.
 * 
 * By your use of Paradyn, you understand and agree that we (or any
 * other person or entity with proprietary rights in Paradyn) are
 * under no obligation to provide either maintenance services,
 * update services, notices of latent defects, or correction of
 * defects for Paradyn.
 * 
 * Even if advised of the possibility of such damages, under no
 * circumstances shall we (or any other person or entity with
 * proprietary rights in the software licensed hereunder) be liable
 * to you or any third party for direct, indirect, or consequential
 * damages of any character regardless of type of action, including,
 * without limitation, loss of profits, loss of use, loss of good
 * will, or computer failure or malfunction.  You agree to indemnify
 * us (and any other person or entity with proprietary rights in the
 * software licensed hereunder) for any and all liability it may
 * incur to third parties resulting from your use of Paradyn.
 */

// $Id: debug.C,v 1.1 2006/12/06 21:22:47 bernat Exp $

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include "common/h/Pair.h"
#include "common/h/Vector.h"
#include "util.h"
#include "BPatch.h"
#include "dyninstAPI/src/debug.h"
#include "EventHandler.h"

// Make a lock.

eventLock *debugPrintLock = NULL;

void showInfoCallback(pdstring msg)
{
    BPatch::reportError(BPatchWarning, 0, msg.c_str());
}

char errorLine[1024];

void showErrorCallback(int num, pdstring msg)
{
    BPatch::reportError(BPatchSerious, num, msg.c_str());
}

void logLine(const char *line)
{
    BPatch::reportError(BPatchWarning, 0, line);
}

// The unused parameter is used by Paradyn's version of this function.
void statusLine(const char *line, bool /* unused */ )
{
    BPatch::reportError(BPatchInfo, 0, line);
}

//  bpfatal, bpsevere, bpwarn, and bpinfo are intended as drop-in 
//  replacements for printf.
#define ERR_BUF_SIZE 2048 // egad -- 1024 is not large enough

int bpfatal(const char *format, ...)
{
  if (NULL == format) return -1;

  char errbuf[ERR_BUF_SIZE];

  va_list va;
  va_start(va, format);
#if defined (i386_unknown_nt4_0)
  _vsnprintf(errbuf, ERR_BUF_SIZE,format, va);
#else
  vsnprintf(errbuf, ERR_BUF_SIZE,format, va);
#endif
  va_end(va);

  BPatch::reportError(BPatchFatal, 0, errbuf);

  return 0;
}


int bpfatal_lf(const char *__file__, unsigned int __line__, const char *format, ...)
{
  fprintf(stderr, "%s[%d]\n", __FILE__, __LINE__);
  if (NULL == format) return -1;

  char errbuf[ERR_BUF_SIZE];

  fprintf(stderr, "%s[%d]\n", __FILE__, __LINE__);
  int header_len = sprintf(errbuf, "[%s]%s[%d]: ", getThreadStr(getExecThreadID()), __file__, __line__);

  fprintf(stderr, "%s[%d]\n", __FILE__, __LINE__);
  va_list va;
  va_start(va, format);
  VSNPRINTF(errbuf + header_len, ERR_BUF_SIZE,format, va);
  va_end(va);

  fprintf(stderr, "%s[%d]\n", __FILE__, __LINE__);
  BPatch::reportError(BPatchFatal, 0, errbuf);

  fprintf(stderr, "%s[%d]\n", __FILE__, __LINE__);
  return 0;
}

int bperr(const char *format, ...)
{
  if (NULL == format) return -1;

  char errbuf[ERR_BUF_SIZE];

  va_list va;
  va_start(va, format);
#if defined (i386_unknown_nt4_0)
  int errbuflen = _vsnprintf(errbuf, ERR_BUF_SIZE, format, va);
#else
  int errbuflen = vsnprintf(errbuf, ERR_BUF_SIZE, format, va);
#endif
  va_end(va);

  char syserr[128];
  if (errno) {
#if defined (i386_unknown_nt4_0)
    int syserrlen = _snprintf(syserr, 128," [%d: %s]", errno, strerror(errno));
#else
    int syserrlen = snprintf(syserr, 128," [%d: %s]", errno, strerror(errno));
#endif
    /* reset errno so that future calls to this function don't report same error */
    errno = 0; 

    if ((errbuflen + syserrlen) < ERR_BUF_SIZE)
      strcat(errbuf, syserr);
    else {
      BPatch::reportError(BPatchSerious, 0, errbuf);
      BPatch::reportError(BPatchSerious, 0, syserr);
      return 0;
    }
  }
  BPatch::reportError(BPatchSerious, 0, errbuf);

  return 0;
}

int bpwarn(const char *format, ...)
{
  if (NULL == format) return -1;

  char errbuf[ERR_BUF_SIZE];

  va_list va;
  va_start(va, format);
#if defined (i386_unknown_nt4_0)
  _vsnprintf(errbuf, ERR_BUF_SIZE,format, va);
#else
  vsnprintf(errbuf, ERR_BUF_SIZE,format, va);
#endif
  va_end(va);

  BPatch::reportError(BPatchWarning, 0, errbuf);

  return 0;
}

int bpinfo(const char *format, ...)
{
  if (NULL == format) return -1;

  char errbuf[ERR_BUF_SIZE];

  va_list va;
  va_start(va, format);
#if defined (i386_unknown_nt4_0)
  _vsnprintf(errbuf, ERR_BUF_SIZE, format, va);
#else
  vsnprintf(errbuf, ERR_BUF_SIZE, format, va);
#endif
  va_end(va);

  BPatch::reportError(BPatchInfo, 0, errbuf);

  return 0;
}


// Internal debugging

int dyn_debug_signal = 0;
int dyn_debug_infrpc = 0;
int dyn_debug_startup = 0;
int dyn_debug_parsing = 0;
int dyn_debug_forkexec = 0;
int dyn_debug_proccontrol = 0;
int dyn_debug_stackwalk = 0;
int dyn_debug_dbi = 0;
int dyn_debug_inst = 0;
int dyn_debug_reloc = 0;
int dyn_debug_dyn_unw = 0;
int dyn_debug_dyn_dbi = 0;
int dyn_debug_mutex = 0;
int dyn_debug_mailbox = 0;
int dyn_debug_async = 0;
int dyn_debug_dwarf = 0;
int dyn_debug_thread = 0;
int dyn_debug_rtlib = 0;
int dyn_debug_catchup = 0;
int dyn_debug_bpatch = 0;
int dyn_debug_regalloc = 0;
int dyn_debug_ast = 0;

bool init_debug() {
  char *p;
  if ( (p=getenv("DYNINST_DEBUG_SIGNAL"))) {
    fprintf(stderr, "Enabling DyninstAPI signal debug\n");
    dyn_debug_signal = 1;
  }
  if ( (p=getenv("DYNINST_DEBUG_INFRPC"))) {
    fprintf(stderr, "Enabling DyninstAPI inferior RPC debug\n");
    dyn_debug_infrpc = 1;
  }
  if ( (p=getenv("DYNINST_DEBUG_INFERIORRPC"))) {
    fprintf(stderr, "Enabling DyninstAPI inferior RPC debug\n");
    dyn_debug_infrpc = 1;
  }
  if ( (p=getenv("DYNINST_DEBUG_STARTUP"))) {
    fprintf(stderr, "Enabling DyninstAPI startup debug\n");
    dyn_debug_startup = 1;
  }
  if ( (p=getenv("DYNINST_DEBUG_PARSING"))) {
    fprintf(stderr, "Enabling DyninstAPI parsing debug\n");
    dyn_debug_parsing = 1;
  }
  if ( (p=getenv("DYNINST_DEBUG_PARSE"))) {
    fprintf(stderr, "Enabling DyninstAPI parsing debug\n");
    dyn_debug_parsing = 1;
  }
  if ( (p=getenv("DYNINST_DEBUG_FORKEXEC"))) {
    fprintf(stderr, "Enabling DyninstAPI forkexec debug\n");
    dyn_debug_forkexec = 1;
  }
  if ( (p=getenv("DYNINST_DEBUG_PROCCONTROL"))) {
    fprintf(stderr, "Enabling DyninstAPI process control debug\n");
    dyn_debug_proccontrol = 1;
  }
  if ( (p=getenv("DYNINST_DEBUG_STACKWALK"))) {
    fprintf(stderr, "Enabling DyninstAPI stack walking debug\n");
    dyn_debug_stackwalk = 1;
  }
  if ( (p=getenv("DYNINST_DEBUG_INST"))) {
    fprintf(stderr, "Enabling DyninstAPI inst debug\n");
    dyn_debug_inst = 1;
  }
  if ( (p=getenv("DYNINST_DEBUG_RELOC"))) {
    fprintf(stderr, "Enabling DyninstAPI relocation debug\n");
    dyn_debug_reloc = 1;
  }
  if ( (p=getenv("DYNINST_DEBUG_RELOCATION"))) {
    fprintf(stderr, "Enabling DyninstAPI relocation debug\n");
    dyn_debug_reloc = 1;
  }
  if ( (p=getenv("DYNINST_DEBUG_DYN_UNW"))) {
    fprintf(stderr, "Enabling DyninstAPI dynamic unwind debug\n");
    dyn_debug_dyn_unw = 1;
    }
  if ( (p=getenv("DYNINST_DEBUG_DBI"))) {
    fprintf(stderr, "Enabling DyninstAPI debugger interface debug\n");
    dyn_debug_dyn_dbi = 1;
    }
  if ( (p=getenv("DYNINST_DEBUG_MUTEX"))) {
    fprintf(stderr, "Enabling DyninstAPI mutex debug\n");
    dyn_debug_mutex = 1;
    }
  if ( (p=getenv("DYNINST_DEBUG_MAILBOX"))) {
    fprintf(stderr, "Enabling DyninstAPI callbacks debug\n");
    dyn_debug_mailbox= 1;
    }
  if ( (p=getenv("DYNINST_DEBUG_DWARF"))) {
    fprintf(stderr, "Enabling DyninstAPI dwarf debug\n");
    dyn_debug_dwarf= 1;
    }
  if ( (p=getenv("DYNINST_DEBUG_ASYNC"))) {
    fprintf(stderr, "Enabling DyninstAPI async debug\n");
    dyn_debug_async= 1;
    }
  if ( (p=getenv("DYNINST_DEBUG_THREAD"))) {
      fprintf(stderr, "Enabling DyninstAPI thread debug\n");
      dyn_debug_thread = 1;
  }
  if ( (p=getenv("DYNINST_DEBUG_RTLIB"))) {
      fprintf(stderr, "Enabling DyninstAPI RTlib debug\n");
      dyn_debug_rtlib = 1;
  }
  if ( (p=getenv("DYNINST_DEBUG_CATCHUP"))) {
      fprintf(stderr, "Enabling DyninstAPI catchup debug\n");
      dyn_debug_catchup = 1;
  }
  if ( (p=getenv("DYNINST_DEBUG_BPATCH"))) {
      fprintf(stderr, "Enabling DyninstAPI bpatch debug\n");
      dyn_debug_bpatch = 1;
  }
  if ( (p=getenv("DYNINST_DEBUG_REGALLOC"))) {
      fprintf(stderr, "Enabling DyninstAPI register allocation debug\n");
      dyn_debug_regalloc = 1;
  }
  if ( (p=getenv("DYNINST_DEBUG_AST"))) {
      fprintf(stderr, "Enabling DyninstAPI ast debug\n");
      dyn_debug_ast = 1;
  }

  debugPrintLock = new eventLock();

  return true;
}

int signal_printf(const char *format, ...)
{
  if (!dyn_debug_signal) return 0;
  if (NULL == format) return -1;

  debugPrintLock->_Lock(FILE__, __LINE__);

  fprintf(stderr, "[%s]", getThreadStr(getExecThreadID()));
  va_list va;
  va_start(va, format);
  int ret = vfprintf(stderr, format, va);
  va_end(va);

  debugPrintLock->_Unlock(FILE__, __LINE__);

  return ret;
}

int inferiorrpc_printf(const char *format, ...)
{
  if (!dyn_debug_infrpc) return 0;
  if (NULL == format) return -1;

  debugPrintLock->_Lock(FILE__, __LINE__);

  fprintf(stderr, "[%s]", getThreadStr(getExecThreadID()));
  va_list va;
  va_start(va, format);
  int ret = vfprintf(stderr, format, va);
  va_end(va);

  debugPrintLock->_Unlock(FILE__, __LINE__);

  return ret;
}

int startup_printf(const char *format, ...)
{
  if (!dyn_debug_startup) return 0;
  if (NULL == format) return -1;

  debugPrintLock->_Lock(FILE__, __LINE__);

  fprintf(stderr, "[%s]", getThreadStr(getExecThreadID()));
  va_list va;
  va_start(va, format);
  int ret = vfprintf(stderr, format, va);
  va_end(va);

  debugPrintLock->_Unlock(FILE__, __LINE__);

  return ret;
}

int parsing_printf(const char *format, ...)
{
  if (!dyn_debug_parsing) return 0;
  if (NULL == format) return -1;

  debugPrintLock->_Lock(FILE__, __LINE__);

  fprintf(stderr, "[%s]", getThreadStr(getExecThreadID()));
  va_list va;
  va_start(va, format);

  int ret = vfprintf(stderr, format, va);

  va_end(va);

  debugPrintLock->_Unlock(FILE__, __LINE__);

  return ret;
}

int forkexec_printf(const char *format, ...)
{
  if (!dyn_debug_forkexec) return 0;
  if (NULL == format) return -1;

  debugPrintLock->_Lock(FILE__, __LINE__);

  fprintf(stderr, "[%s]", getThreadStr(getExecThreadID()));
  va_list va;
  va_start(va, format);
  int ret = vfprintf(stderr, format, va);
  va_end(va);

  debugPrintLock->_Unlock(FILE__, __LINE__);

  return ret;
}

int proccontrol_printf(const char *format, ...)
{
  if (!dyn_debug_proccontrol) return 0;
  if (NULL == format) return -1;

  debugPrintLock->_Lock(FILE__, __LINE__);

  fprintf(stderr, "[%s]", getThreadStr(getExecThreadID()));
  va_list va;
  va_start(va, format);
  int ret = vfprintf(stderr, format, va);
  va_end(va);

  debugPrintLock->_Unlock(FILE__, __LINE__);

  return ret;
}

int stackwalk_printf(const char *format, ...)
{
  if (!dyn_debug_stackwalk) return 0;
  if (NULL == format) return -1;

  debugPrintLock->_Lock(FILE__, __LINE__);

  fprintf(stderr, "[%s]", getThreadStr(getExecThreadID()));
  va_list va;
  va_start(va, format);
  int ret = vfprintf(stderr, format, va);
  va_end(va);

  debugPrintLock->_Unlock(FILE__, __LINE__);

  return ret;
}



int inst_printf(const char *format, ...)
{
  if (!dyn_debug_inst) return 0;
  if (NULL == format) return -1;

  debugPrintLock->_Lock(FILE__, __LINE__);

  fprintf(stderr, "[%s]", getThreadStr(getExecThreadID()));
  va_list va;
  va_start(va, format);
  int ret = vfprintf(stderr, format, va);
  va_end(va);

  debugPrintLock->_Unlock(FILE__, __LINE__);

  return ret;
}

int reloc_printf(const char *format, ...)
{
  if (!dyn_debug_reloc) return 0;
  if (NULL == format) return -1;

  debugPrintLock->_Lock(FILE__, __LINE__);
  
  fprintf(stderr, "[%s]", getThreadStr(getExecThreadID()));
  va_list va;
  va_start(va, format);
  int ret = vfprintf(stderr, format, va);
  va_end(va);

  debugPrintLock->_Unlock(FILE__, __LINE__);

  return ret;
}

int dyn_unw_printf(const char *format, ...)
{
  if (!dyn_debug_dyn_unw ) return 0;
  if (NULL == format) return -1;

  debugPrintLock->_Lock(FILE__, __LINE__);
  
  fprintf(stderr, "[%s]", getThreadStr(getExecThreadID()));
  va_list va;
  va_start(va, format);
  int ret = vfprintf(stderr, format, va);
  va_end(va);

  debugPrintLock->_Unlock(FILE__, __LINE__);

  return ret;
}

int dbi_printf(const char *format, ...)
{
  if (!dyn_debug_dyn_dbi ) return 0;
  if (NULL == format) return -1;

  debugPrintLock->_Lock(FILE__, __LINE__);
  
  fprintf(stderr, "[%s]", getThreadStr(getExecThreadID()));
  va_list va;
  va_start(va, format);
  int ret = vfprintf(stderr, format, va);
  va_end(va);

  debugPrintLock->_Unlock(FILE__, __LINE__);

  return ret;
}

int mutex_printf(const char *format, ...)
{
  if (!dyn_debug_mutex ) return 0;
  if (NULL == format) return -1;

  debugPrintLock->_Lock(FILE__, __LINE__);
  
  fprintf(stderr, "[%s]", getThreadStr(getExecThreadID()));
  va_list va;
  va_start(va, format);
  int ret = vfprintf(stderr, format, va);
  va_end(va);

  debugPrintLock->_Unlock(FILE__, __LINE__);

  return ret;
}

int mailbox_printf(const char *format, ...)
{
  if (!dyn_debug_mailbox ) return 0;
  if (NULL == format) return -1;

  debugPrintLock->_Lock(FILE__, __LINE__);
  
  fprintf(stderr, "[%s]", getThreadStr(getExecThreadID()));
  va_list va;
  va_start(va, format);
  int ret = vfprintf(stderr, format, va);
  va_end(va);

  debugPrintLock->_Unlock(FILE__, __LINE__);

  return ret;
}

int async_printf(const char *format, ...)
{
  if (!dyn_debug_async ) return 0;
  if (NULL == format) return -1;

  debugPrintLock->_Lock(FILE__, __LINE__);
  
  fprintf(stderr, "[%s]", getThreadStr(getExecThreadID()));
  va_list va;
  va_start(va, format);
  int ret = vfprintf(stderr, format, va);
  va_end(va);

  debugPrintLock->_Unlock(FILE__, __LINE__);

  return ret;
}

int dwarf_printf(const char *format, ...)
{
  if (!dyn_debug_dwarf ) return 0;
  if (NULL == format) return -1;

  debugPrintLock->_Lock(FILE__, __LINE__);
  
  fprintf(stderr, "(dwarf) [thread %s]: ", getThreadStr(getExecThreadID()));
  va_list va;
  va_start(va, format);
  int ret = vfprintf(stderr, format, va);
  va_end(va);

  debugPrintLock->_Unlock(FILE__, __LINE__);

  return ret;
}


int thread_printf(const char *format, ...)
{
  if (!dyn_debug_thread) return 0;
  if (NULL == format) return -1;

  debugPrintLock->_Lock(FILE__, __LINE__);
  
  fprintf(stderr, "[%s]: ", getThreadStr(getExecThreadID()));
  va_list va;
  va_start(va, format);
  int ret = vfprintf(stderr, format, va);
  va_end(va);

  debugPrintLock->_Unlock(FILE__, __LINE__);

  return ret;
}


int catchup_printf(const char *format, ...)
{

    if (!dyn_debug_catchup) return 0;
    if (NULL == format) return -1;
    
    debugPrintLock->_Lock(FILE__, __LINE__);
    
    fprintf(stderr, "[%s]: ", getThreadStr(getExecThreadID()));
    va_list va;
    va_start(va, format);
    int ret = vfprintf(stderr, format, va);
    va_end(va);
    
    debugPrintLock->_Unlock(FILE__, __LINE__);
    
    return ret;
}

int bpatch_printf(const char *format, ...)
{
  if (!dyn_debug_bpatch) return 0;
  if (NULL == format) return -1;

  debugPrintLock->_Lock(FILE__, __LINE__);
  
  fprintf(stderr, "[%s]: ", getThreadStr(getExecThreadID()));
  va_list va;
  va_start(va, format);
  int ret = vfprintf(stderr, format, va);
  va_end(va);

  debugPrintLock->_Unlock(FILE__, __LINE__);

  return ret;
}

int regalloc_printf(const char *format, ...)
{
  if (!dyn_debug_regalloc) return 0;
  if (NULL == format) return -1;

  debugPrintLock->_Lock(FILE__, __LINE__);
  
  fprintf(stderr, "[%s]: ", getThreadStr(getExecThreadID()));
  va_list va;
  va_start(va, format);
  int ret = vfprintf(stderr, format, va);
  va_end(va);

  debugPrintLock->_Unlock(FILE__, __LINE__);

  return ret;
}

int ast_printf(const char *format, ...)
{
  if (!dyn_debug_ast) return 0;
  if (NULL == format) return -1;

  debugPrintLock->_Lock(FILE__, __LINE__);
  
  fprintf(stderr, "[%s]: ", getThreadStr(getExecThreadID()));
  va_list va;
  va_start(va, format);
  int ret = vfprintf(stderr, format, va);
  va_end(va);

  debugPrintLock->_Unlock(FILE__, __LINE__);

  return ret;
}

StatContainer stats_instru;
StatContainer stats_ptrace;
StatContainer stats_parse;
StatContainer stats_codegen;

TimeStatistic running_time;

bool have_stats = 0;

bool init_stats() {
    char *p;
    running_time.start();

    if ((p = getenv("DYNINST_STATS_INST"))) {
        fprintf(stderr, "Enabling DyninstAPI instrumentation statistics\n");
        stats_instru.add(INST_GENERATE_TIMER, TimerStat);
        stats_instru.add(INST_INSTALL_TIMER, TimerStat);
        stats_instru.add(INST_LINK_TIMER, TimerStat);
        stats_instru.add(INST_REMOVE_TIMER, TimerStat);
        stats_instru.add(INST_GENERATE_COUNTER, CountStat);
        stats_instru.add(INST_INSTALL_COUNTER, CountStat);
        stats_instru.add(INST_LINK_COUNTER, CountStat);
        stats_instru.add(INST_REMOVE_COUNTER, CountStat);
        have_stats = true;
    }
    

    if ((p = getenv("DYNINST_STATS_PTRACE"))) {
        fprintf(stderr, "Enabling DyninstAPI ptrace statistics\n");
        stats_ptrace.add(PTRACE_WRITE_TIMER, TimerStat);
        stats_ptrace.add(PTRACE_READ_TIMER, TimerStat);

        stats_ptrace.add(PTRACE_WRITE_COUNTER, CountStat);
        stats_ptrace.add(PTRACE_READ_COUNTER, CountStat);

        stats_ptrace.add(PTRACE_WRITE_AMOUNT, CountStat);
        stats_ptrace.add(PTRACE_READ_AMOUNT, CountStat);
        have_stats = true;
    }
    
    if ((p = getenv("DYNINST_STATS_PARSING"))) {
        fprintf(stderr, "Enabling DyninstAPI parsing statistics\n");
        stats_parse.add(PARSE_SYMTAB_TIMER, TimerStat);
        stats_parse.add(PARSE_ANALYZE_TIMER, TimerStat);
        have_stats = true;
    }

    if ((p = getenv("DYNINST_STATS_CODEGEN"))) {
        fprintf(stderr, "Enabling DyninstAPI code generation statistics\n");

        stats_codegen.add(CODEGEN_AST_TIMER, TimerStat);
        stats_codegen.add(CODEGEN_AST_COUNTER, CountStat);
        stats_codegen.add(CODEGEN_REGISTER_TIMER, TimerStat);
        stats_codegen.add(CODEGEN_LIVENESS_TIMER, TimerStat);
        have_stats = true;
    }
    return have_stats;
}

bool print_stats() {
    char *p;

    running_time.stop();
    if (have_stats) 
        fprintf(stderr, "Running time: %f sec (user), %f sec (system), %f sec (wall)\n",
                running_time.usecs(),
                running_time.ssecs(),
                running_time.wsecs());
    running_time.start();

    if ((p = getenv("DYNINST_STATS_INST"))) {
        fprintf(stderr, "Printing DyninstAPI instrumentation statistics\n");
        fprintf(stderr, "  Generation: %ld calls, %f sec (user), %f sec (system), %f sec (wall)\n",
                stats_instru[INST_GENERATE_COUNTER]->value(),
                stats_instru[INST_GENERATE_TIMER]->usecs(),
                stats_instru[INST_GENERATE_TIMER]->ssecs(),
                stats_instru[INST_GENERATE_TIMER]->wsecs());
        fprintf(stderr, "  Installation: %ld calls, %f sec (user), %f sec (system), %f sec (wall)\n",
                stats_instru[INST_INSTALL_COUNTER]->value(),
                stats_instru[INST_INSTALL_TIMER]->usecs(),
                stats_instru[INST_INSTALL_TIMER]->ssecs(),
                stats_instru[INST_INSTALL_TIMER]->wsecs());
        fprintf(stderr, "  Linking: %ld calls, %f sec (user), %f sec (system), %f sec (wall)\n",
                stats_instru[INST_LINK_COUNTER]->value(),
                stats_instru[INST_LINK_TIMER]->usecs(),
                stats_instru[INST_LINK_TIMER]->ssecs(),
                stats_instru[INST_LINK_TIMER]->wsecs());
        fprintf(stderr, "  Removal: %ld calls, %f sec (user), %f sec (system), %f sec (wall)\n",
                stats_instru[INST_REMOVE_COUNTER]->value(),
                stats_instru[INST_REMOVE_TIMER]->usecs(),
                stats_instru[INST_REMOVE_TIMER]->ssecs(),
                stats_instru[INST_REMOVE_TIMER]->wsecs());
    }

    if ((p = getenv("DYNINST_STATS_PTRACE"))) {
        fprintf(stderr, "Printing DyninstAPI ptrace statistics\n");
        fprintf(stderr, "  Write: %ld calls, %ld bytes, %f sec (user), %f sec (system), %f sec (wall)\n",
                stats_ptrace[PTRACE_WRITE_COUNTER]->value(),
                stats_ptrace[PTRACE_WRITE_AMOUNT]->value(),
                stats_ptrace[PTRACE_WRITE_TIMER]->usecs(),
                stats_ptrace[PTRACE_WRITE_TIMER]->ssecs(),
                stats_ptrace[PTRACE_WRITE_TIMER]->wsecs());

        fprintf(stderr, "  Read: %ld calls, %ld bytes, %f sec (user), %f sec (system), %f sec (wall)\n",
                stats_ptrace[PTRACE_READ_COUNTER]->value(),
                stats_ptrace[PTRACE_READ_AMOUNT]->value(),
                stats_ptrace[PTRACE_READ_TIMER]->usecs(),
                stats_ptrace[PTRACE_READ_TIMER]->ssecs(),
                stats_ptrace[PTRACE_READ_TIMER]->wsecs());
    }

    if ((p = getenv("DYNINST_STATS_PARSING"))) {
        fprintf(stderr, "Printing DyninstAPI parsing statistics\n");
        fprintf(stderr, "  Symtab parsing:  %f sec (user), %f sec (system), %f sec (wall)\n",
                stats_parse[PARSE_SYMTAB_TIMER]->usecs(),
                stats_parse[PARSE_SYMTAB_TIMER]->ssecs(),
                stats_parse[PARSE_SYMTAB_TIMER]->wsecs());
        fprintf(stderr, "  Analysis:  %f sec (user), %f sec (system), %f sec (wall)\n",
                stats_parse[PARSE_ANALYZE_TIMER]->usecs(),
                stats_parse[PARSE_ANALYZE_TIMER]->ssecs(),
                stats_parse[PARSE_ANALYZE_TIMER]->wsecs());
    }

    if ((p = getenv("DYNINST_STATS_CODEGEN"))) {
        fprintf(stderr, "Printing DyninstAPI code generation statistics\n");
        fprintf(stderr, "  AST generation: %ld calls, %f sec (user), %f sec (system), %f sec (wall)\n",
                stats_codegen[CODEGEN_AST_COUNTER]->value(),
                stats_codegen[CODEGEN_AST_TIMER]->usecs(),
                stats_codegen[CODEGEN_AST_TIMER]->ssecs(),
                stats_codegen[CODEGEN_AST_TIMER]->wsecs());

        fprintf(stderr, "  Register allocation: %f sec (user), %f sec (system), %f sec (wall)\n",
                stats_codegen[CODEGEN_REGISTER_TIMER]->usecs(),
                stats_codegen[CODEGEN_REGISTER_TIMER]->ssecs(),
                stats_codegen[CODEGEN_REGISTER_TIMER]->wsecs());
        
        fprintf(stderr, "  Liveness analysis: %f sec (user), %f sec (system), %f sec (wall)\n",
                stats_codegen[CODEGEN_LIVENESS_TIMER]->usecs(),
                stats_codegen[CODEGEN_LIVENESS_TIMER]->ssecs(),
                stats_codegen[CODEGEN_LIVENESS_TIMER]->wsecs());
    }
    return true;
}
