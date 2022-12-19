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

 
Node * add_newNode(Node* head, pid_t new_pid, char * new_path){
	Node* new_node = (Node*) malloc(sizeof(Node));
	new_node->pid  = new_pid;
	new_node->path = new_path;
	new_node->next = head;
	return new_node;
}

Node * deleteNode(Node* head, pid_t pid){
	Node* curr = head;
	Node* prev = NULL;

	if(curr->pid == pid){
		return curr->next;
	}
	
	while(curr != NULL){
		if(curr->pid == pid){
			prev->next = curr->next;
			return head;
		}
		else{
			prev = curr;
			curr = curr->next;
		}
	}
	return head;
}

void printList(Node *node){
	Node* curr = node;
	int i = 0;

	while(curr != NULL){
		printf("%d: %s\n", curr->pid, curr->path);
		curr = curr->next;
		i++;
	}
	printf("Total background jobs:  %d\n", i);
}


int PifExist(Node *node, pid_t pid){
	// if pid is in linked list return 0 if not return -1
	Node* curr = node;

	while(curr != NULL){
		if(curr->pid == pid){
			return 0;
		}
		else{
			curr = curr->next;
		}
	}
  	return -1;
}

pid_t getPid(Node *head){
	Node* curr = head;

	if(curr == NULL){
		return -1;
	}
	else{
		return curr->pid;
	}
}
