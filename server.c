#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

#define KEY 22342

int newadress(){
    int adress;
    //check existing
    return adress;
}

bool serch(char text[50], int file){
    switch(file){
        case 1: //patrz, czy jest temat
            char filename[50];
            snprintf(filename, sizeof(filename), "user/%s", text);
            int fd = open(filename, O_RDONLY);
            if (fd == -1) {
                return false;
            }
            close(fd);
            return true;
            break;
        case 2: //patrz, czy jest użytkownik
            char filename[50];
            snprintf(filename, sizeof(filename), "user/ban%s", text);
            int fd = open(filename, O_RDONLY);
            if (fd == -1) {
                return false;
            }
            close(fd);
            return true;
            break;
        default:
            return NULL;
    }
}

void login(char name[50], int address){
    if(!search(name, 2)){
        //make new file banname and subname
        chdir("user");
        const char sub[] = "sub";
        const char ban[] = "ban";
        int subsize = sizeof(sub) + sizeof(name);
        int bansize = sizeof(ban) + sizeof(name);
        char result[subsize]; //sub + name
        char result2[bansize]; //ban + name
        strcpy(result, sub);
        strcat(result, name);
        strcpy(result2, ban);
        strcat(result2, name);
        creat(result, 0777);
        int file = creat(result2, 0644);
        snd.type = 1;
        address = adress + KEY;
        char buf[30];
        snprintf(buf, sizeof(buf), "%d", address);
        strcat(buf, "\n");
        write(file, buf, sizeof(buf));
        chdir("..");
    }
    else{
        snd.type = 2;
    }
    msgsnd(address, &snd, sizeof(snd), 0);
    msgctl(address, IPC_RMID, NULL);
}

void logout(char name[50]){
    //remove adress and name
    snd.type = 2;
    msgsnd(address, &snd, sizeof(snd), 0);
    msgctl(address, IPC_RMID, NULL);
}
void addtopic(){

}
void addtext(){

}
void addsub(){

}
void blockuser(){
    
}
void senderr(int address){
    snd.type = 12;
    msgsnd(address, &snd, sizeof(snd), 0);
    msgctl(address, IPC_RMID, NULL);
}

int main(int argc, char* argv[]) {
    struct msgbuf snd {
        long type;
        char text[256];
        int num;
    }
    struct msgbuf2 rec {
        long type;
        int adress;
        char top[50]; //topic or name
        char text[256];
    }

    int msgid = msgget(0x111111, 0666 | IPC_CREAT);
    //generate data structure
    if (mkdir("data", 0777) == -1) {
        printf("Error creating data folder");
        return 1;
    }
    if (chdir("data") == -1) {
        printg("Error changing to data directory");
        return 1l;
    }
    if (mkdir("text", 0777) == -1) {
        printf("Error creating text folder");
        return 1;
    }
    if (mkdir("user", 0777) == -1) {
        printf("Error creating user folder");
        return 1;
    }
    while(1){
        msgrcv(msgid, &rec, sizeof(rec) - sizeof(long), 0, 0);
        if(!fork()){
            switch(rec.type){
                case 1:
                    login(rec.top, rec.address);
                    break;
                case 2:
                    logout();
                    break;
                case 3:
                    addtopic();
                    break;
                case 4:
                    addtext();
                    break;
                case 5:
                    addsub();
                    break;
                case 6:
                    blockuser();
                    break;
                default:
                    senderr(int user);
            }
        }
    }


}