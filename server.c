#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
//#include <sys/ipc.h>
//#include <sys/msg.h>
#include <sys/types.h>

#define KEY 22342

int search(char text[50], int file) {
    switch(file) {
        case 1: // patrz, czy jest temat
        {
            char filename[50];
            snprintf(filename, sizeof(filename), "text/%s", text);
            int fd = open(filename, O_RDONLY);
            if (fd == -1) {
                return 0;
            }
            close(fd);
            return 1;
        }
        case 2: // patrz, czy jest użytkownik - by adress
        {
            char filename[50];
            snprintf(filename, sizeof(filename), "user/ban%s", text);
            int fd = open(filename, O_RDONLY);
            if (fd == -1) {
                return 0;
            }
            close(fd);
            return 1;
        }
        case 3: //patrz, czy jest użytkownik - by name
        {
            const char users[] = "users";
            int fd = open(users, O_RDONLY);
            char reader[50];
            char c;
            int counter = 0;
            int timeout = 0;
            while(timeout < 100000){
                timeout++;
                read(fd, &c, 1);
                if(c == '\0'){
                    close(fd);
                    return 0;
                }
                else if(c == '\n'){
                    reader[counter] = '\0';
                    if(strcmp(reader, text) == 0){
                        close(fd);
                        return 1;
                    }
                    else{
                        counter = 0;
                        memset(reader, '\0', sizeof(reader)); 
                    }
                }
                else{
                    reader[counter] = c;
                    counter++;
                }
            }
            close(fd);
            return 0;
        }
        default:
            return 0;
    }
}

void sendtosub(char text[]){ //asynchroniczne

}


void login(char name[], int address) {
    char buf[30];
    address = address + KEY; //Key - stala duża liczba
    snprintf(buf, sizeof(buf), "%d", address);
    if (!search(buf, 2)) {
        //add adress + name to users
        const char users[] = "users";
        const char newline[] = "\n";
        int pd = open(users, O_WRONLY);
        lseek(pd, 0, SEEK_END);
        write(pd, buf, sizeof(buf)-1);
        write(pd, newline, sizeof(newline)-1);
        write(pd, name, sizeof(name)-1);
        write(pd, newline, sizeof(newline)-1);
        close(pd);
        // make new file banname and subname
        chdir("user");
        const char sub[] = "sub";
        const char ban[] = "ban";
        int subsize = sizeof(sub) + sizeof(buf);
        int bansize = sizeof(ban) + sizeof(buf);
        char result[subsize]; // sub + adress
        char result2[bansize]; // ban + adress
        strcpy(result, sub);
        strcat(result, buf);
        strcpy(result2, ban);
        strcat(result2, buf);
        int file = creat(result, 0777);
        close(file);
        file = creat(result2, 0777);
        snd.type = 1;
        strcat(buf, "\n");
        write(file, name, strlen(name));
        close(file);
        chdir("..");
    } else {
        snd.type = 2;
    }

    msgsnd(address, &snd, sizeof(snd), 0);
    msgctl(address, IPC_RMID, NULL);
}

void logout(char name[], int address){
    //remove adress and name
    char buf[30];
    address = address + KEY; //Key - stala duża liczba
    snprintf(buf, sizeof(buf), "%d", address);
    if(search(buf, 2)){
        const char users[] = "users";
        const char newline[] = "\n";
        int pd = open(users, O_RDWR);
        char reader[50];
        int notdone = 1;
        int counter = 0;
        char c;
        while(notdone){
            read(pd, &c, 1);
            if(c == '\n'){
                reader[counter] == '\0';
                if(strcmp(buf, reader)){
                    notdone = 0;
                }
                else{
                    counter = 0;
                    memset(reader, '\0', sizeof(reader));
                }
            }
            else if(c == '\0'){
                printf("Error in logout")
                snd.type = 2;
                msgsnd(address, &snd, sizeof(snd), 0);
                msgctl(address, IPC_RMID, NULL);
                return;
            }
            else{
                reader[counter] = c;
                counter++;
            }
        }
        lseek(pd, -counter, SEEK_CUR);
        const char emp[] = "";
        for(int i = 0; i < counter; i++){
            write(pd, emp, 1);
        }
        for(int i = 0; i < (strlen(name) + 1); i++){
            write(pd, emp, 1);
        }
        close(pd);
        chdir("user");
        const char sub[] = "sub";
        const char ban[] = "ban";
        int subsize = sizeof(sub) + sizeof(buf);
        int bansize = sizeof(ban) + sizeof(buf);
        char result[subsize]; //sub + name
        char result2[bansize]; //ban + name
        strcpy(result, sub);
        strcat(result, buf);
        strcpy(result2, ban);
        strcat(result2, buf);
        remove(result);
        remove(result2);
        snd.type = 11;
    }
    else{
        snd.type = 2;
    }
    msgsnd(address, &snd, sizeof(snd), 0);
    msgctl(address, IPC_RMID, NULL);
}
void addtopic(char title[], char text[], int address, int pro){
    char buf[30];
    address = address + KEY; //Key - stala duża liczba
    snprintf(buf, sizeof(buf), "%d", address);
    const char hash = '#';
    const char app = '&';
    const char newl = '\n';
    char buf2[30];
    const char tmp[] = "text";
    const char users[] = "users";
    snprintf(buf2, sizeof(buf2), "%d", pro);
    if(search(buf, 2)){
        //get name
        char name[50];
        int fd = open(users, O_RDONLY);
        char c;
        int counter2 = 0;
        int notdone = 1;
        while(notdone){
            read(fd, &c, 1);
            if(c == '\n'){
                name[counter2] = '\0';
                if(strcmp(name, buf) == 0){
                    notdone = 0;
                }
                else{
                    counter2 = 0;
                    memset(name, '\0', sizeof(name));
                }
            }
            else{
                name[counter2] = c;
                counter2++;
            }
        }
        notdone = 1;
        counter2 = 0;
        memset(name, '\0', sizeof(name));
        while(notdone){
            read(fd, &c, 1);
            if(c == '\n'){
                notdone = 0;
            }
            else{
                name[counter2] = c;
                counter2++;
            }
        }
        name[counter2] = '\0';
        if(search(title, 1)){ //jeśli już jest temat
            chdir(tmp);
            int pd = open(title, O_RDWR);
            int notdone = 1;
            char c;
            char reader[50];
            int counter;
            int foundnumber = 0;
            while(notdone){
                read(pd, &c, 1);
                if(foundnumber && !(c == '\n')){
                    reader[counter] = c;
                    counter++;
                }
                else{
                    reader[counter] = '\0';
                    foundnumber = atoi(reader);
                    if(foundnumber >= pro){
                        notdone = 0;
                    }
                    else{
                        foundnumber = 0;
                        counter = 0;
                        memset(reader, '\0', sizeof(reader));
                    }
                }
                if(c == '#'){
                    foundnumber = 1;
                }
            }
            lseek(pd, -(counter+1), SEEK_CUR);
            write(pd, &hash, 1);
            write(pd, buf2, sizeof(buf2)-1);
            write(pd, &app, 1);
            write(pd, name, sizeof(name)-1);
            write(pd, &newl, 1);
            write(pd, text, sizeof(text)-1);
            write(pd, &newl, 1);
            close(pd);
            chdir("..");
            snd.type = 4;
            sendtosub(title);
        }
        else{ //jeśli nie ma
            const char topics[] = "topics";
            chdir(tmp);
            int pd = creat(title, 0777);
            write(pd, &hash, 1);
            write(pd, buf2, sizeof(buf2)-1);
            write(pd, &app, 1);
            write(pd, name, sizeof(name)-1);
            write(pd, &newl, 1);
            write(pd, text, sizeof(text)-1);
            write(pd, &newl, 1);
            close(pd);
            snd.type = 5;
            chdir("..");
            //add to list of all topic
            pd = open(topics, O_RDWR);
            lseek(pd, 0, SEEK_END);
            write(pd, title, sizeof(title) -1);
            write(pd, &newl, 1);
            close(pd);
        }
    }
    else{
        snd.type = 6;
    }
    msgsnd(address, &snd, sizeof(snd), 0);
    msgctl(address, IPC_RMID, NULL);
}
void addsub(char test[], int address){
    char buf[30];
    address = address + KEY; //Key - stala duża liczba
    snprintf(buf, sizeof(buf), "%d", address);
    const char newl = '\n';
    const char tmp[] = "user";
    const char sub[] = "sub";
    int subsize = sizeof(sub) + sizeof(buf);
    char result[subsize]; //sub + name
    strcpy(result, sub);
    strcat(result, buf);
    if(search(buf, 2)){
        if(serach(test, 1)){
            chdir(tmp);
            int pd = open(result, O_WRONLY);
            lseek(pd, 0, SEEK_END);
            write(pd, test, sizeof(test)-1);
            write(pd, &newl, 1);
            close(pd);
            chdir("..");
            snd.type = 7;
        }
        else{
            snd.type = 8;
        }
    }
    else{
        snd.type = 14;
    }
    msgsnd(address, &snd, sizeof(snd), 0);
    msgctl(address, IPC_RMID, NULL);
}
void blockuser(char test[], int address){
    char buf[30];
    address = address + KEY; //Key - stala duża liczba
    snprintf(buf, sizeof(buf), "%d", address);
    const char newl = '\n';
    const char tmp[] = "user";
    const char sub[] = "ban";
    int subsize = sizeof(sub) + sizeof(buf);
    char result[subsize]; //sub + name
    strcpy(result, sub);
    strcat(result, buf);
    if(search(buf, 2)){
        if(serach(test, 3)){
            chdir(tmp);
            int pd = open(result, O_WRONLY);
            lseek(pd, 0, SEEK_END);
            write(pd, test, sizeof(test)-1);
            write(pd, &newl, 1);
            close(pd);
            chdir("..");
            snd.type = 7;
        }
        else{
            snd.type = 8;
        }
    }
    else{
        snd.type = 14;
    }
    msgsnd(address, &snd, sizeof(snd), 0);
    msgctl(address, IPC_RMID, NULL);
}
void senderr(int address){
    snd.type = 12;
    msgsnd(address, &snd, sizeof(snd), 0);
    msgctl(address, IPC_RMID, NULL);
}

void getmsg(char text[], int num, int address){ //synchroniczne

}

struct msgbuf {
        long type;
        char text[256];
        int num;
}snd;
struct msgbuf {
        long type;
        int address;
        int pro; //prioriyty
        char top[50]; //topic or name
        char text[256];
}rec;
int main(int argc, char* argv[]) {

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
    int pd;
    char users[] = "users";
    pd = creat(users, 0777);
    if(pd == -1){
        printf("Error creating list of users");
        return 1;
    }
    char topics[] = "topics";
    pd = creat(topics, 0777);
    if(pd == -1){
        printf("Error creating list of topics");
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
                    logout(rec.top, rec.address);
                    break;
                case 3:
                    addtopic(rec.top, rec.text, rec.address, rec.pro);
                    break;
                case 4:
                    getmsg(rec.top, rec.pro, rec.address);
                    break;
                case 5:
                    addsub(rec.top, rec.pro);
                    break;
                case 6:
                    blockuser(rec.top, rec.pro);
                    break;
                default:
                    senderr(rec.address);
            }
        }
    }


}