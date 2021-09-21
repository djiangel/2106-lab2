/**
 * CS2106 AY21/22 Semester 1 - Lab 2
 *
 * This file contains function definitions. Your implementation should go in
 * this file.
 */

#include "myshell.h"
#include <stdio.h>
#include<stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct PROCESS {
    int pid;
    int exit_status;
    int status;
    struct PROCESS *next;
} process;

typedef struct {
    process *head;
} list;

list *lst;

void addToLinkedList(list *lst, int pid, int status, int exit_status);
void updateProcesses(list *lst);


void my_init(void) {
    // Initialize what you need here
    lst = (list *)malloc(sizeof(list));
    lst->head = NULL;
}

void my_process_command(size_t num_tokens, char **tokens) {
    // Your code here, refer to the lab document for a description of the arguments
    int pid;
    updateProcesses(lst);
    if (strcmp(tokens[0], "info") == 0) {
        if (lst->head == NULL) {
            return;
        } else {
            process *current = lst->head;
            while (current != NULL) {
                if (current->status == 0) {
                    printf("[%d] Exited %d\n", current->pid, current->exit_status);
                } else {
                    printf("[%d] Running\n", current->pid);
                }
                current = current->next;
            }
        }
        return;
    } else if (strcmp(tokens[0], "wait") == 0) {
        int status;
        char** end;
        printf("in wait");
        int processPid = (int) strtol(tokens[1], end, 10);
        if (waitpid(processPid, &status, WNOHANG) != -1) {
            printf("inside here");
            waitpid(processPid, &status, 0);
        }
        return;
    }
    if (access(*tokens, F_OK) == 0) {
        pid = fork();
        if (pid == 0) {
                execv(tokens[0], &tokens[0]);
        } else {
            int status;
            if (strcmp(tokens[num_tokens - 2], "&") != 0) {
                waitpid(pid, &status, 0);
                if (WIFEXITED(status))
                {
                    int exit_status = WEXITSTATUS(status);      
                    addToLinkedList(lst, pid, 0, exit_status);
                }
            } else {
                printf("Child[%i] in background\n", pid);
                addToLinkedList(lst, pid, 1, NULL);       
            }
        }
    } else {
        printf("%s not found\n", tokens[0]);
    }

}

void my_quit(void) {
    // Clean up function, called after "quit" is entered as a user command
    printf("Goodbye!");
}

void addToLinkedList(list *lst, int pid, int status, int exit_status) {
    process *p = (process*) malloc(sizeof(process));
    p->pid = pid;
    p->status = status;
    p->exit_status = exit_status;
    if (lst->head == NULL) {
        lst->head = p;
    } else {
        process *temp;
        temp = lst->head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = p;
    }
    p->next = NULL;
}

void updateProcesses(list *lst) {
    if (lst->head == NULL) {
        return;
    }
    process *p = lst->head;
    int status;
    while (p != NULL) {
        if (p->status != 0) {
            pid_t return_pid = waitpid(p->pid, &status, WNOHANG); /* WNOHANG def'd in wait.h */
            if (return_pid == -1) {
                /* error */
                printf("error\n");
            } else if (return_pid == 0) {
                /* child is still running */
                printf("still running \n");
            } else if (return_pid == p->pid) {
                /* child is finished. exit status in status */
                p->status = 0;
                if (WIFEXITED(status))
                {
                    int exit_status = WEXITSTATUS(status);     
                    p->exit_status = exit_status;
                }
            }
        }
        p = p->next;
    }
}