#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <pwd.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

pthread_mutex_t lock;

void eraseFirstAndLastCharacter(char* str) {
    int length = strlen(str);
    if (length >= 2) {
        // Shift characters to the left to remove the first character
        for (int i = 0; i < length - 1; ++i) {
            str[i] = str[i + 1];
        }
        // Erase the last character by setting it to null terminator
        str[length - 2] = '\0';
    } else {
        // Handle cases where the string has 0 or 1 characters
        str[0] = '\0';
    }
}


void print_process_info(const char *pid, char *user, char *procname, char *state) {
    char* username;
    int atoiUser = atoi(user);
    if(atoiUser == 0) {
        username = "root";
    } else {
        struct passwd *pw = getpwuid(atoiUser);
        username = pw->pw_name;
    }
    printf("| %-10s | %-10s | %-20s | %-10s |\n", pid, username, procname, state);
}

void read_proc_stat(const char *pid) {
    char path[50];
    snprintf(path, sizeof(path), "/proc/%s/stat", pid);

    FILE *file = fopen(path, "r");
    if (file == NULL) {
        return;
    }

    char procname[256], state;
    fscanf(file, "%*s %s %c", procname, &state);
    eraseFirstAndLastCharacter(procname);
    fclose(file);

    char user[50];
    snprintf(path, sizeof(path), "/proc/%s/status", pid);
    file = fopen(path, "r");
    if (file == NULL) {
        return;
    }

    while (fscanf(file, "%*s %s", user) != EOF) {
        if (strcmp("Uid:", user) == 0) {
            fscanf(file, "%s", user);
            break;
        }
    }

    fclose(file);

    print_process_info(pid, user, procname, (state == 'R') ? "Running" : "Sleeping");
}

void* display_process() {
    DIR *dir;
    dir = opendir("/proc");

    if (dir == NULL) {
        perror("Error opening directory");
        return NULL;
    }

    while(1) {
        pthread_mutex_lock(&lock);
        fflush(stdout);
        
        printf("|------------|------------|----------------------|------------|\n");
        printf("|    PID     |    User    |       PROCNAME       |   Estado   |\n");
        printf("|------------|------------|----------------------|------------|\n");

        struct dirent *entry;

        int i=0;

        while ((entry = readdir(dir)) != NULL && i<40) {
            // Check if the entry is a directory and represents a process ID
            const char *pidc = entry->d_name;
            if (entry->d_type == DT_DIR && atoi(pidc) != 0) {
                read_proc_stat(pidc);
                i++;
            }

        }
        printf("|------------|------------|----------------------|------------|\n");
        
        rewinddir(dir);
        
        fflush(stdout);
        pthread_mutex_unlock(&lock);
        sleep(1);
    }
    closedir(dir);
}


void* get_signal() {
    while (1) {
        sleep(2);
        pthread_mutex_lock(&lock);
        int pid, signal;
        scanf("%d %d", &pid, &signal);

        int result = kill(pid, signal);
        if (result == 0) {
            printf("Signal %d sent to process with PID %d.\n", signal, pid);
        } else {
            perror("Error sending signal");
        }
        pthread_mutex_unlock(&lock);
    }
}

int main() {
    pthread_t threadDisplay;
    pthread_t threadInput; 

    if (pthread_mutex_init(&lock, NULL) != 0) { 
        printf("\n mutex init has failed\n"); 
        return 1; 
    } 
    
    if (pthread_create(&threadDisplay, NULL, display_process, NULL)) {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&threadInput, NULL, get_signal, NULL)) {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }

    pthread_join(threadDisplay, NULL);
    pthread_join(threadInput, NULL);
    
    return 0;
}