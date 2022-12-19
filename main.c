/*
 * Assignment 1 Fall 2022 CSC360
 *
 * Author: Jonathan Cote V00962634
 * Last Modified: 2022-10-03
*/

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include "linked_list.h"

Node *head = NULL;

/*
 * Function: check_process_status
 * --------------------------------
 * checks processes status for if it has exited or been terminated.
 *    if exited or terminated it cleans up the process and removes from linked-list
 * 
 * pid: child processes pid you want to check status for (-1 if you want to check all child processes)
 * 
 * return: if detection of process being exited/terminated returns 1
 *         if process has not been detected as exited/terminated return 0
 *         if error occured return -1
*/
int check_process_status(pid_t pid){
  int retVal, status;

  retVal = waitpid(pid, &status, WNOHANG);
  if(retVal == -1){
    return -1;
  }
  else if(WIFEXITED(status) && retVal > 0){
      if(WEXITSTATUS(status) == EXIT_FAILURE){
        return 1;
      }
      else{
      printf("Cleaner: > Process %d had exited (Cleaning Up)\n", retVal);
      if((PifExist(head, retVal)) == 0){
        head = deleteNode(head, retVal);
      }
      return 1;
      }
  }
  else if(WIFSIGNALED(status) && retVal > 0){
    printf("Cleaner: > Process %d had been killed (Cleaning Up)\n", retVal);
    if((PifExist(head, retVal)) == 0){
        head = deleteNode(head, retVal);
    }
    return 1;
  }
  else{
    return 0;
  }
}

/*
 * Function: func_BG
 * --------------------------------
 * creates a child process that executes given program. if execution of given program is successful parent adds
 *   child pid and execute file path to linked-list.
 * 
 * cmd: execution file name and all of the neccessary arguments for that file
*/
void func_BG(char **cmd)
{
  pid_t pid;
  pid = fork();
  if (pid < 0)
  {
    printf("Error occurred when making new process.\n");
  }
  else if (pid == 0)
  {
    char file[100];
    sprintf(file, "./%s", cmd[1]);
    char* const* args = cmd + 1;
		if(execvp(file, args) < 0)
    {
      perror("Error on execvp"); 
      exit(EXIT_FAILURE); 
    }
  }
  else
  { 
    usleep(10000);
    int retVal;
    retVal = check_process_status(pid);
    if(retVal == 0){
      char *path = realpath(cmd[1], NULL);
      if(path != NULL){
        head = add_newNode(head, pid, path);
        printf("Process created with pid: %d\n", pid);
      }
    }
  }
}

/*
 * Function: func_BGlist
 * --------------------------------
 * prints out all currently running child processes tracked in the linked-list
*/
void func_BGlist()
{
  int retVal;
  retVal = check_process_status(-1);

  if(retVal == -1){
    printList(head);
  }
  else{
    while(retVal == 1){
      retVal = check_process_status(-1);
    }
    printf("*****************************\n");
    printList(head);
    printf("*****************************\n\n");
  }
}

/*
 * Function: func_BGkill
 * --------------------------------
 * executes the kill command with the terminate signal to terminate a child process with given pid
 * 
 * cmd: char representation of pid for child process
*/
void func_BGkill(char **cmd)
{
  pid_t pid = atoi(cmd[1]);
  int retVal;

  if((PifExist(head, pid)) == 0){
    if(kill(pid, SIGTERM) == 0){
      retVal = check_process_status(pid);

      if(retVal == 0){
        head = deleteNode(head, pid);
        printf("termination signal sent to pid: %d\n", pid);
      }
      else if(retVal == 1){
        printf("(bgkill aborted)\n");
      }
    }
    else{
      printf("Error occured during termination of pid: %d\n", pid);
    }
  }
  else{
    printf("pid: %d, does not exist\n", pid);
  }
}

/*
 * Function: func_BGstop
 * --------------------------------
 * executes the kill command with the stop signal to stop a child process with given pid
 * 
 * cmd: char representation of pid for child process
*/
void func_BGstop(char **cmd)
{
  pid_t pid = atoi(cmd[1]);
  int retVal;

  if((PifExist(head, pid)) == 0){
    if(kill(pid, SIGSTOP) == 0){
      retVal = check_process_status(pid);
      
      if(retVal == 0){
        printf("Background job Stopped with pid: %d\n", pid);
      }
      else if(retVal == 1){
        printf("(bgstop aborted)\n");
      } 
    }
    else{
      printf("Error occured when attempting to stop pid: %d\n", pid);
    }
  }
  else{
    printf("pid: %d, does not exist\n", pid);
  }
}

/*
 * Function: func_BGstart
 * --------------------------------
 * executes the kill command with the continue signal to start a child process with given pid
 * 
 * cmd: char representation of pid for child process
*/
void func_BGstart(char **cmd)
{
  pid_t pid = atoi(cmd[1]);
  int retVal;

  if((PifExist(head, pid)) == 0){
    if(kill(pid, SIGCONT) == 0){
      retVal = check_process_status(pid);

      if(retVal == 0){
        printf("background job Started with pid: %d\n", pid);
      }
      else if(retVal == 1){
        printf("(bgstart aborted)\n");
      }
    }
    else{
      printf("Error occured when attempting to start pid: %d\n", pid);
    }
  }
  else{
    printf("pid: %d, does not exist\n", pid);
  }
}

/*
 * Function: print_pstat
 * --------------------------------
 * prints out process's stats in a user friendly way
 * 
 * Inputs are the processes stats wanting to be printed:
 *    comm: char*
 *    state: char
 *    utime: float
 *    stime: float
 *    rss: long int
 *    voluntary_ctxt_switches: int
 *    nonvoluntary_ctxt_switches: int
 *    pid: pid_t
*/
void print_pstat(char *comm, char state, float utime, float stime, long int rss,
  int voluntary_ctxt_switches, int nonvoluntary_ctxt_switches, pid_t pid){

  printf("Process (%d): Stats\n", pid);
  printf("**********************************************\n");
  printf("comm: %-10s\nstate: %c\nutime: %f\nstime: %f\nrss: %ld\nvoluntary_ctxt_switches: %d\nnonvoluntary_ctxt_switches: %d\n", 
    comm, state, utime, stime, rss, voluntary_ctxt_switches, nonvoluntary_ctxt_switches);
  printf("**********************************************\n\n");        

}

/*
 * Function: func_pstat
 * --------------------------------
 * collect and print out the stats for a given pid
 * 
 * cmd: char representation of pid for child process
*/
void func_pstat(char *str_pid)
{
  pid_t pid = atoi(str_pid);

  if((PifExist(head, pid)) == 0){
    char filename[255];
    sprintf(filename, "/proc/%d/stat", pid);
    FILE *file = fopen(filename, "r");

    char comm[255], state;
    float utime, stime;
    long int rss;

    unsigned int unused_u;
    int unused_d;
    unsigned long unused_lu;
    unsigned long long unused_llu;
    long int unused_ld;

    fscanf(file, "%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %f %f %ld %ld %ld %ld %ld %ld %llu %lu %ld",
            &unused_d, comm, &state, &unused_d, &unused_d, &unused_d, &unused_d, &unused_d, &unused_u, &unused_lu,
            &unused_lu, &unused_lu, &unused_lu, &utime, &stime, &unused_ld, &unused_ld, &unused_ld, &unused_ld,
            &unused_ld, &unused_ld, &unused_llu, &unused_lu, &rss);

    fclose(file);
    
    sprintf(filename, "/proc/%d/status", pid);
    file = fopen(filename, "r");
    char line[255];
    char key[255];
    char value[255];
    
    int voluntary_ctxt_switches, nonvoluntary_ctxt_switches;

    while(fgets(line, 255, file) != NULL){
      sscanf(line, "%[A-Za-z0-9_]: %[ A-Za-z0-9()/-,]", key, value);

      if(!strcmp(key, "voluntary_ctxt_switches")){
        voluntary_ctxt_switches = atoi(value);
      }
      else if(!strcmp(key, "nonvoluntary_ctxt_switches")){
        nonvoluntary_ctxt_switches = atoi(value);
      }
    }
    fclose(file);

    print_pstat(comm, state, utime, stime, rss, voluntary_ctxt_switches, nonvoluntary_ctxt_switches, pid);

  }
  else{
    printf("Invalid input, please input a pid running in pman\n");
  }

}

int main()
{
  char user_input_str[50];
  while (true)
  {
    printf("Pman: > ");
    fgets(user_input_str, 50, stdin);
    printf("User input: %s \n", user_input_str);
    char *ptr = strtok(user_input_str, " \n");
    if(ptr == NULL){
      continue;
    }
    char *lst[50];
    int index = 0;
    lst[index] = ptr;
    index++;
    while (ptr != NULL)
    {
      ptr = strtok(NULL, " \n");
      lst[index] = ptr;
      index++;
    }
    if (strcmp("bg", lst[0]) == 0 && lst[1] != NULL)
    {
      func_BG(lst);
    }
    else if (strcmp("bglist", lst[0]) == 0)
    {
      func_BGlist();
    }
    else if (strcmp("bgkill", lst[0]) == 0 && lst[1] != NULL)
    {
      func_BGkill(lst);
    }
    else if (strcmp("bgstop", lst[0]) == 0 && lst[1] != NULL)
    {
      func_BGstop(lst);
    }
    else if (strcmp("bgstart", lst[0]) == 0 && lst[1] != NULL)
    {
      func_BGstart(lst);
    }
    else if (strcmp("pstat", lst[0]) == 0 && lst[1] != NULL)
    {
      func_pstat(lst[1]);
    }
    else if (strcmp("q", lst[0]) == 0)
    {
      pid_t pid;
      char s_pid[100];
      printf("Cleaning up\n");
      printf("*****************************\n");
      while((pid = getPid(head)) != -1){
        sprintf(s_pid, "%d", pid);
        lst[1] = s_pid;
        func_BGkill(lst);
      }
      printf("*****************************\n");
      printf("Cleanup Complete, Good Bye\n"); 
      exit(0);
    }
    else
    {
      printf("Invalid input\n");
    }
    
  }

  return 0;
}
