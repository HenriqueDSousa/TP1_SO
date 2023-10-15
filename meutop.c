#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <pwd.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <termios.h>
#include <fcntl.h>

pthread_mutex_t lock;


int kbhit() {
    struct termios oldt, newt;
    int ch;
    int fd = fileno(stdin);
    int flags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    fcntl(fd, F_SETFL, flags);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    if( ch == '\n')
        return 0;

    else 
       return ch != EOF;
}

void eraseFirstAndLastCharacter(char* str) {
    int length = strlen(str);
    if (length >= 2) {
        for (int i = 0; i < length - 1; ++i) {
            str[i] = str[i + 1];
        }
        str[length - 2] = '\0';
    } else {
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
        if (pw != NULL) {
            username = pw->pw_name;
        } else {
            // Handle the case where getpwuid returns NULL
            username = "unknown";
        }
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

   
        pthread_mutex_lock(&lock);

        printf("\033[H\033[J");
        fflush(stdout);
        
        printf("|------------|------------|----------------------|------------|\n");
        printf("|    PID     |    User    |       PROCNAME       |   Estado   |\n");
        printf("|------------|------------|----------------------|------------|\n");

        struct dirent *entry;

        int i=0;

        while ((entry = readdir(dir)) != NULL && i<20) {
            const char *pidc = entry->d_name;
            if (entry->d_type == 4 && atoi(pidc) != 0) {
                read_proc_stat(pidc);
                i++;
            }

        }
        printf("|------------|------------|----------------------|------------|\n");
        printf("Pressione uma tecla (exceto Enter) para mandar um sinal para um processo\n");
        
        rewinddir(dir);
        
        fflush(stdout);
        
        sleep(1);
        pthread_mutex_unlock(&lock);
    
    closedir(dir);
}


void* get_signal() {

  
        if(kbhit()) {
            
            printf("\n");
            pthread_mutex_lock(&lock);
            int pid, signal;
            scanf("%d %d", &pid, &signal);

            int result = kill(pid, signal);
            printf("result kill = %d", result);
            if (result == 0) {
                printf("Signal %d sent to process with PID %d.\n", signal, pid);
            } else {
                perror("Error sending signal");
            }
            
            sleep(1);
            pthread_mutex_unlock(&lock);

            
    }
}

int main() {
    
    while(1) { 
        pthread_t threadDisplay;
        pthread_t threadInput; 

        if (pthread_mutex_init(&lock, NULL) != 0) { 
            printf("\n mutex init has failed\n"); 
            exit(EXIT_FAILURE);
        } 

        if (pthread_create(&threadDisplay, NULL, display_process, NULL)) {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }

        sleep(1);
        if (pthread_create(&threadInput, NULL, get_signal, NULL)) {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }

        pthread_join(threadDisplay, NULL);
        pthread_join(threadInput, NULL);
   } 

    pthread_mutex_destroy(&lock);
    return 0;
}