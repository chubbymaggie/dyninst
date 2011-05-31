/*
 * Copyright (c) 1996-2011 Barton P. Miller
 * 
 * We provide the Paradyn Parallel Performance Tools (below
 * described as "Paradyn") on an AS IS basis, and do not warrant its
 * validity or performance.  We reserve the right to update, modify,
 * or discontinue this software at any time.  We shall have no
 * obligation to supply such updates or modifications or any other
 * form of support to you.
 * 
 * By your use of Paradyn, you understand and agree that we (or any
 * other person or entity with proprietary rights in Paradyn) are
 * under no obligation to provide either maintenance services,
 * update services, notices of latent defects, or correction of
 * defects for Paradyn.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
#include "mutatee_util.h"

/* Externally accessed function prototypes.  These must have globally unique
 * names.  I suggest following the pattern <testname>_<function>
 */

void test_sparc_3_func();
void test_sparc_3_call();

/* Global variables accessed by the mutator.  These must have globally unique
 * names.
 */

/* Internally used function prototypes.  These should be declared with the
 * keyword static so they don't interfere with other mutatees in the group.
 */

static void call0();

/* Global variables used internally by the mutatee.  These should be declared
 * with the keyword static so they don't interfere with other mutatees in the
 * group.
 */

static volatile int passed = 0;

/* Function definitions follow */

// I hope this works here..
// moved after func4 to make sure it appears to be "outside" func3
asm("           .text");
asm("label3:");
asm("		retl");
asm("		nop");
asm("		call abort");
asm("		nop");

//
// use call; load %07 with current PC 
//
asm("		.stabs  \"test_sparc_3_func:F(0,1)\",36,0,16,test_sparc_3_func");
asm("		.global test_sparc_3_func");
asm("		.type   test_sparc_3_func,#function");
asm("test_sparc_3_func:");
asm("		save %sp, -112, %sp");
asm("		nop");
asm("		nop");
asm("		call label3");		// external call to set pc into o7
asm("		nop");
asm("		call call0");
asm("		nop");
asm("		ret");
asm("		restore");

void call0()
{
}

void test_sparc_3_call()
{
  passed = 1;
  logerror("\nPassed test #3\n");
}

int test_sparc_3_mutatee() {
  test_sparc_3_func();
  if (1 == passed) {
    test_passes(testname);
    return 0; /* Test passed */
  } else {
    return -1; /* Test failed */
  }
}