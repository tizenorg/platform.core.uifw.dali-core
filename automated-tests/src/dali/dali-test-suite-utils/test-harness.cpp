/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "test-harness.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <map>
#include <cstring>
#include <testcase.h>

namespace TestHarness
{

typedef std::map<int, TestCase> RunningTestCases;


int RunTestCase( struct ::testcase_s& testCase )
{
  int result = EXIT_STATUS_TESTCASE_FAILED;

  try
  {
    if( testCase.startup )
    {
      testCase.startup();
    }
    result = testCase.function();
    if( testCase.cleanup )
    {
      testCase.cleanup();
    }
  }
  catch (...)
  {
    printf("Caught exception in test case.\n");
    result = EXIT_STATUS_TESTCASE_ABORTED;
  }

  return result;
}

int RunTestCaseInChildProcess( struct ::testcase_s& testCase, bool suppressOutput )
{
  int testResult = EXIT_STATUS_TESTCASE_FAILED;

  int pid = fork();
  if( pid == 0 ) // Child process
  {
    if( suppressOutput )
    {
      close(STDOUT_FILENO);
      close(STDERR_FILENO);
    }
    exit( RunTestCase( testCase ) );
  }
  else if(pid == -1)
  {
    perror("fork");
    exit(EXIT_STATUS_FORK_FAILED);
  }
  else // Parent process
  {
    int status = 0;
    int childPid = waitpid(-1, &status, 0);
    if( childPid == -1 )
    {
      perror("waitpid");
      exit(EXIT_STATUS_WAITPID_FAILED);
    }
    if( WIFEXITED(status) )
    {
      if( childPid > 0 )
      {
        testResult = WEXITSTATUS(status);
        if( testResult )
        {
          printf("Test case %s failed: %d\n", testCase.name, testResult);
        }
      }
    }
    else if(WIFSIGNALED(status) )
    {
      testResult = EXIT_STATUS_TESTCASE_ABORTED;

#ifdef WCOREDUMP
      if(WCOREDUMP(status))
      {
        printf("Test case %s crashed\n", testCase.name);
      }
#endif
      printf("Test case %s exited with signal %s\n", testCase.name, strsignal(WTERMSIG(status)));
    }
    else if(WIFSTOPPED(status))
    {
      printf("Test case %s stopped with signal %s\n", testCase.name, strsignal(WSTOPSIG(status)));
    }
  }
  return testResult;
}

int RunAll(const char* processName, ::testcase tc_array[], bool reRunFailed)
{
  int numFailures = 0;
  int numPasses = 0;

  // Run test cases in child process( to kill output/handle signals ), but run serially.
  for( unsigned int i=0; tc_array[i].name; i++)
  {
    int result = RunTestCaseInChildProcess( tc_array[i], true );
    if( result == 0 )
    {
      numPasses++;
    }
    else
    {
      numFailures++;
    }
  }

  printf("\rNumber of test passes: %d\n", numPasses);
  printf("Number of test failures: %d\n", numFailures);

  return numFailures;
}



// Constantly runs up to MAX_NUM_CHILDREN processes
int RunAllInParallel(  const char* processName, ::testcase tc_array[], bool reRunFailed)
{
  int numFailures = 0;
  int numPasses = 0;

  RunningTestCases children;
  std::vector<int> failedTestCases;

  // Fork up to MAX_NUM_CHILDREN processes, then
  // wait. As soon as a proc completes, fork the next.

  int nextTestCase = 0;
  int numRunningChildren = 0;

  while( tc_array[nextTestCase].name || numRunningChildren > 0)
  {
    // Create more children (up to the max number or til the end of the array)
    while( numRunningChildren < MAX_NUM_CHILDREN && tc_array[nextTestCase].name )
    {
      int pid = fork();
      if( pid == 0 ) // Child process
      {
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
        exit( RunTestCase( tc_array[nextTestCase] ) );
      }
      else if(pid == -1)
      {
        perror("fork");
        exit(EXIT_STATUS_FORK_FAILED);
      }
      else // Parent process
      {
        TestCase tc(nextTestCase, tc_array[nextTestCase].name);
        children[pid] = tc;
        nextTestCase++;
        numRunningChildren++;
      }
    }

    // Wait for the next child to finish

    int status=0;
    int childPid = waitpid(-1, &status, 0);
    if( childPid == -1 )
    {
      perror("waitpid");
      exit(EXIT_STATUS_WAITPID_FAILED);
    }

    if( WIFEXITED(status) )
    {
      if( childPid > 0 )
      {
        int testResult = WEXITSTATUS(status);
        if( testResult )
        {
          printf("Test case %s failed: %d\n", children[childPid].testCaseName, testResult);
          failedTestCases.push_back(children[childPid].testCase);
          numFailures++;
        }
        else
        {
          numPasses++;
        }
        numRunningChildren--;
      }
    }

    else if( WIFSIGNALED(status) || WIFSTOPPED(status))
    {
      status = WIFSIGNALED(status)?WTERMSIG(status):WSTOPSIG(status);

      if( childPid > 0 )
      {
        RunningTestCases::iterator iter = children.find(childPid);
        if( iter != children.end() )
        {
          printf("Test case %s exited with signal %s\n", iter->second.testCaseName, strsignal(status));
          failedTestCases.push_back(iter->second.testCase);
        }
        else
        {
          printf("Unknown child process: %d signaled %s\n", childPid, strsignal(status));
        }

        numFailures++;
        numRunningChildren--;
      }
    }
  }

  printf("\rNumber of test passes: %d                                        \n", numPasses);
  printf("Number of test failures: %d\n", numFailures);

  if( reRunFailed )
  {
    for( unsigned int i=0; i<failedTestCases.size(); i++)
    {
      char* testCaseStrapline;
      int numChars = asprintf(&testCaseStrapline, "Test case %s", tc_array[failedTestCases[i]].name );
      printf("\n%s\n", testCaseStrapline);
      for(int j=0; j<numChars; j++)
      {
        printf("=");
      }
      printf("\n");
      RunTestCaseInChildProcess( tc_array[failedTestCases[i] ], false );
    }
  }

  return numFailures;
}



int FindAndRunTestCase(::testcase tc_array[], const char* testCaseName)
{
  int result = EXIT_STATUS_TESTCASE_NOT_FOUND;

  for( int i = 0; tc_array[i].name; i++ )
  {
    if( !strcmp(testCaseName, tc_array[i].name) )
    {
      return RunTestCase( tc_array[i] );
    }
  }

  printf("Unknown testcase name: \"%s\"\n", testCaseName);
  return result;
}

void Usage(const char* program)
{
  printf("Usage: \n"
         "   %s <testcase name>\t\t Execute a test case\n"
         "   %s \t\t Execute all test cases in parallel\n"
         "   %s -r\t\t Execute all test cases in parallel, rerunning failed test cases\n",
         program, program, program);
}

} // namespace
