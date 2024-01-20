#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define KEY 22342
struct msgbuf1 {
        long type;
        char text[256];
        int num;
}snd;

struct msgbuf {
        long type;
        int address;
        int pro; //priority
        char top[50]; //topic or name
        char text[256];
}rec;

int search(char text[], int file) {
    switch(file) {
        case 1: // patrz, czy jest temat
        {   
            const char users[] = "topics";
            int fd = open(users, O_RDONLY);
            char reader[50];
            char c;
            int counter = 0;
            int timeout = 0;
            while(read(fd, &c, 1) > 0){
                if(c == '\n'){
                    reader[counter] = '\0';
                    if(strstr(reader, text) != NULL){
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
        case 2: // patrz, czy jest użytkownik - by adress
        {
            const char users[] = "users";
            int fd = open(users, O_RDONLY);
            char reader[50];
            char c;
            int counter = 0;
            int timeout = 0;
            while(read(fd, &c, 1) > 0){
                if(c == '\n'){
                    reader[counter] = '\0';
                    if(strstr(reader, text) != NULL){
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
        case 3: //patrz, czy jest użytkownik - by name
        {
            const char users[] = "users";
            int fd = open(users, O_RDONLY);
            char reader[50];
            char c;
            int counter = 0;
            int timeout = 0;
            while(read(fd, &c, 1) > 0){
                timeout++;
                if(c == '\0'){
                    close(fd);
                    return 0;
                }
                else if(c == '\n'){
                    reader[counter] = '\0';
                    if(strstr(reader, text) != NULL){
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

int substotopic(char who[], char what[]){
    chdir("user");
    const char sub[] = "sub";
    int subsize = strlen(sub) + strlen(who);
    char result[subsize]; // sub + adress
    strcpy(result, sub);
    strcat(result, who);
    int pd = open(result, O_RDONLY);
    char reed[50];
    char c;
    int counter = 1;
    while(read(pd, &c, 1) > 0){
        read(pd, &c, 1);
        if(c == '\n'){
            reed[counter] = '\0';
            if(strstr(what, reed) != NULL){
                close(pd);
                chdir("..");
                return 1;
            }
            else{
                counter = 0;
                memset(reed, '\0', sizeof(reed));
            }
        }
        else{
            reed[counter] = c;
            counter++;
        }
    }
    close(pd);
    chdir("..");
    return 0;
}
int notbanned(char who[], char whom[]){
    chdir("user");
    const char sub[] = "ban";
    int subsize = strlen(sub) + strlen(who);
    char result[subsize]; // sub + adress
    strcpy(result, sub);
    strcat(result, who);
    int pd = open(result, O_RDONLY);
    char reed[50];
    char c;
    int counter = 1;
    while(1){
        read(pd, &c, 1);
        if(c == '\n'){
            reed[counter] = '\0';
            if(strcmp(whom, reed) == 0){
                close(pd);
                chdir("..");
                return 0;
            }
            else{
                counter = 0;
                memset(reed, '\0', sizeof(reed));
            }
        }
        else if(c == '\0'){
            close(pd);
            chdir("..");
            return 1;
        }
        else{
            reed[counter] = c;
            counter++;
        }
    }
    close(pd);
    chdir("..");
    return 1;
}
void sendtosub(char text[], char name[], char msg[]){ //asynchroniczne
    const char users[] = "users";
    int pd = open(users, O_RDONLY);
    char address[30];
    char c;
    int counter = 0;
    int notdone = 1;
    int switcher = 1;
    const char newl = '\n';
    while(notdone){
        read(pd, &c, 1);
        if(c == '\n'){
            if(switcher){
                address[counter] = '\0';
                if(substotopic(address, text)){
                    if(notbanned(address, name)){
                        int smsg = strlen(text) + strlen(msg) + 1;
                        char cmplmsg[smsg];
                        strcpy(cmplmsg, name);
                        strcat(cmplmsg, &newl);
                        strcat(cmplmsg, msg);
                        snd.type = 12;
                        snd.num = 0;
                        strcpy(snd.text, cmplmsg);
                        int send = msgget(atoi(address), 0666 | IPC_CREAT);
                        msgsnd(send, &snd, sizeof(snd), 0);
                        msgctl(send, IPC_RMID, NULL);
                    }
                }
                counter = 0;
                memset(address, '\0', sizeof(address));
                switcher = 0;
            }
            else{
                counter = 0;
                memset(address, '\0', sizeof(address));
                switcher = 1;
            }
        }
        else if(c == '\0'){
            close(pd);
            return;
        }
        else{
            address[counter] = c;
            counter++;
        }
    }
}


void login(char name[], int address) {
    struct msgbuf1 snd;
    char buf[30];
    address = address + KEY; //Key - stala duża liczba
    snprintf(buf, sizeof(buf), "%d", address);
    if (!search(name, 3)) {
        //add adress + name to users
        const char users[] = "users";
        const char newline[] = "\n";
        int pd = open(users, O_WRONLY);
        lseek(pd, 0, SEEK_END);
        write(pd, buf, strlen(buf));
        write(pd, newline, strlen(newline));
        write(pd, name, strlen(name));
        write(pd, newline, strlen(newline));
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
        printf("login succesful");
        strcat(buf, "\n");
        write(file, name, strlen(name));
        close(file);
        chdir("..");
    } else {
        snd.type = 2;
        printf("login failed");
    }
    int send = msgget(address, 0666 | IPC_CREAT);
    msgsnd(send, &snd, sizeof(snd), 0);
    msgctl(send, IPC_RMID, NULL);
}

void logout(char name[], int address){
    struct msgbuf1 snd;
    //remove adress and name
    char buf[30];
    address = address + KEY; //Key - stala duża liczba
    snprintf(buf, sizeof(buf), "%d", address);
    if(search(name, 3)){
        const char users[] = "users";
        const char tempusers[] = "temp";
        const char newline[] = "\n";
        int pd = open(users, O_RDWR);
        int pd2 = creat(tempusers, 0777);
        char reader[50];
        memset(reader, '\0', sizeof(reader));
        int notdone = 1;
        int counter = 0;
        int counter2 = 0;
        char c;
        while((read(pd, &c, 1) > 0) && notdone){
            printf("%c", c);
            if(c == '\n'){
                counter2++;
                reader[counter] = '\0';
                if(strstr(name, reader) != NULL){
                    printf("THE GOLDEN ARC\n");
                    notdone = 0;
                }
                else{
                    counter = 0;
                    memset(reader, '\0', sizeof(reader));
                }
            }
            else{
                reader[counter] = c;
                counter++;
                counter2++;
            }
        }
        close(pd);
        pd = open(users, O_RDONLY);
        for(int i = 0; i < (counter2 - (strlen(buf) + strlen(name) + 2)); i++){
            read(pd, &c, 1);
            write(pd2, &c, 1);
            printf("%c", c);
        }
        lseek(pd, (strlen(buf) + strlen(name) + 2), SEEK_CUR);
        while(read(pd, &c, 1) > 0){
            write(pd2, &c, 1);
            printf("%c", c);
        }
        close(pd);
        close(pd2);
        remove(users);
        rename(tempusers, users);
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
        printf("logout succesful\n");
    }
    else{
        snd.type = 2;
        printf("logout failed, but its here\n");
    }
    int send = msgget(address, 0666 | IPC_CREAT);
    msgsnd(send, &snd, sizeof(snd), 0);
    printf("logout conf sent\n");
    msgctl(send, IPC_RMID, NULL);
}
void addtopic(char title[], char text[], int address, int pro){
    struct msgbuf1 snd;
    char buf[30];
    address = address + KEY; //Key - stala duża liczba
    snprintf(buf, sizeof(buf), "%d", address);
    const char hash[] = "#";
    const char app[] = "&";
    const char newl[] = "\n";
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
        while(notdone && (read(fd, &c, 1) > 0)){
            if(c == '\n'){
                name[counter2] = '\0';
                if(strstr(name, buf) != NULL){
                    notdone = 0;
                }
                else{
                    counter2 = 0;
                    memset(name, '\0', strlen(name));
                }
            }
            else{
                name[counter2] = c;
                counter2++;
            }
        }
        notdone = 1;
        counter2 = 0;
        memset(name, '\0', strlen(name));
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
        if(search(title, 1) == 1){ //jeśli już jest temat
            chdir(tmp);
            int pd = open(title, O_RDWR);
            int notdone = 1;
            char c;
            char reader[50];
            int counter;
            int foundnumber = 0;
            while(notdone && read(pd, &c, 1) > 0){
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
            write(pd, hash, strlen(hash));
            write(pd, buf2, strlen(buf2));
            write(pd, app, strlen(app));
            write(pd, name, strlen(name));
            write(pd, newl, strlen(newl));
            write(pd, text, strlen(text));
            write(pd, newl, strlen(newl));
            close(pd);
            chdir("..");
            snd.type = 4;
            printf("message added succesfully");
            sendtosub(title, name, text);
        }
        else{ //jeśli nie ma
            const char topics[] = "topics";
            chdir(tmp);
            int pd = creat(title, 0777);
            write(pd, hash, strlen(hash));
            write(pd, buf2, strlen(buf2));
            write(pd, app, strlen(app));
            write(pd, name, strlen(name));
            write(pd, newl, strlen(newl));
            write(pd, text, strlen(text));
            write(pd, newl, strlen(newl));
            close(pd);
            snd.type = 5;
            printf("topic added succesfully\n");
            chdir("..");
            //add to list of all topic
            pd = open(topics, O_RDWR);
            lseek(pd, 0, SEEK_END);
            write(pd, title, strlen(title));
            write(pd, &newl, 1);
            close(pd);
        }
    }
    else{
        snd.type = 6;
        printf("topic adding failed, %d not found\n", address);
    }
    int send = msgget(address, 0666 | IPC_CREAT);
    msgsnd(send, &snd, sizeof(snd), 0);
    msgctl(send, IPC_RMID, NULL);
}
void addsub(char test[], int address){
    struct msgbuf1 snd;
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
        if(search(test, 1)){
            chdir(tmp);
            int pd = open(result, O_WRONLY);
            lseek(pd, 0, SEEK_END);
            write(pd, test, strlen(test));
            write(pd, &newl, 1);
            close(pd);
            chdir("..");
            snd.type = 7;
            printf("subscription added successfully");
        }
        else{
            snd.type = 8;
            printf("failed to add subsciprtion\n");
        }
    }
    else{
        snd.type = 14;
        printf("failed to add subsciprtion\n");
    }
    int send = msgget(address, 0666 | IPC_CREAT);
    msgsnd(send, &snd, sizeof(snd), 0);
    msgctl(send, IPC_RMID, NULL);
}
void blockuser(char test[], int address){
    struct msgbuf1 snd;
    char buf[50];
    address = address + KEY; //Key - stala duża liczba
    snprintf(buf, sizeof(buf)-1, "%d", address);
    const char newl = '\n';
    const char tmp[] = "user";
    const char sub[] = "ban";
    int subsize = sizeof(sub) + sizeof(buf);
    char result[subsize]; //sub + name
    strcpy(result, sub);
    strcat(result, buf);
    if(search(buf, 2)){
        if(search(test, 3)){
            chdir(tmp);
            int pd = open(result, O_WRONLY);
            lseek(pd, 0, SEEK_END);
            write(pd, test, strlen(test));
            write(pd, &newl, 1);
            close(pd);
            chdir("..");
            snd.type = 7;
            printf("user blocked succesfully\n");
        }
        else{
            snd.type = 8;
            printf("user block failed\n");
        }
    }
    else{
        snd.type = 14;
        printf("user block failed\n");
    }
    int send = msgget(address, 0666 | IPC_CREAT);
    msgsnd(send, &snd, sizeof(snd), 0);
    msgctl(send, IPC_RMID, NULL);
}
void senderr(int address){
    struct msgbuf1 snd;
    snd.type = 12;
    printf("error message sent to user\n");
    int send = msgget(address, 0666 | IPC_CREAT);
    msgsnd(send, &snd, sizeof(snd), 0);
    msgctl(send, IPC_RMID, NULL);
}


void getmsg(char text[], int num, int address){ //synchroniczne
    struct msgbuf1 snd;
    chdir("text");
    address = address + KEY;
    int pd = open(text, O_RDONLY);
    int counter = 0;
    int counter2 = 0;
    char c;
    char name[50];
    char reed[256];
    int whatread = 0;
    while((counter2 < num) && (read(pd, &c, 1) > 0)){
        if(c == '&'){
            whatread = 1; //czuytamy imie
        }
        if(whatread == 1){
            if(c == '\n'){
                name[counter] = '\0';
                counter = 0;
                whatread = 2; //czytamy wiadomość
            }
            else{
                name[counter] = c;
                counter++;
            }
        }
        if(whatread == 2){
            if(c == '#' && !(counter2 = num)){ // koniec tego tematu
                reed[counter] = '\0';
                counter = 0;
                snd.type = 12;
                printf("message sent 1\n");
                snd.num = 1;
                strcpy(snd.text, reed);
                int send = msgget(address, 0666 | IPC_CREAT);
                msgsnd(send, &snd, sizeof(snd), 0);
                msgctl(send, IPC_RMID, NULL);
                counter2++;
                memset(reed, '\0', sizeof(reed));
            }
            else if((c == '#' && !(counter2 == num)) || (c == '\0')){
                reed[counter] = '\0';
                snd.type = 12;
                printf("message sent 1\n");
                snd.num = 0;
                strcpy(snd.text, reed);
                int send = msgget(address, 0666 | IPC_CREAT);
                msgsnd(send, &snd, sizeof(snd), 0);
                msgctl(send, IPC_RMID, NULL);
            }
            else{
                reed[counter] = c;
                counter ++;
            }
        }
    }
    close(pd);
    chdir("..");
}

void givenames(int address){
    struct msgbuf1 snd;
    address = address + KEY;
    const char users[] = "users";
    int pd = open(users, O_RDONLY);
    char reader[50];
    char c;
    int notdone = 1;
    int counter = 0;
    int name = 0;
    while(notdone && (read(pd, &c, 1)>0)){
        read(pd, &c, 1);
        if(c == '\n'){
            if(name){
                reader[counter] = '\0';
                counter = 0;
                snd.type = 16;
                snd.num = 1;
                strcpy(snd.text, reader);
                int send = msgget(address, 0666 | IPC_CREAT);
                msgsnd(send, &snd, sizeof(snd), 0);
                msgctl(send, IPC_RMID, NULL);
                memset(reader, '\0', sizeof(reader));
                name = 0;
            }
            else{
                name = 1;
                counter = 0;
                memset(reader, '\0', sizeof(reader));
            }
        }
        else{
            reader[counter] = c;
            counter++;
        }
    }
    reader[counter] = '\0';
    snd.type = 15;
    snd.num = 0;
    strcpy(snd.text, reader);
    int send = msgget(address, 0666 | IPC_CREAT);
    msgsnd(send, &snd, sizeof(snd), 0);
    msgctl(send, IPC_RMID, NULL);
    close(pd);
    printf("user list sent\n");
}
void givetopics(int address){
    struct msgbuf1 snd;
    address = address + KEY;
    const char topics[] = "topics";
    int pd = open(topics, O_RDONLY);
    char reader[50];
    char c;
    int notdone = 1;
    int counter = 0;
    while(notdone && (read(pd, &c, 1) > 0)){
        if(c == '\n'){
            reader[counter] = '\0';
            counter = 0;
            snd.type = 16;
            snd.num = 1;
            strcpy(snd.text, reader);
            int send = msgget(address, 0666 | IPC_CREAT);
            msgsnd(send, &snd, sizeof(snd), 0);
            msgctl(send, IPC_RMID, NULL);
            memset(reader, '\0', sizeof(reader));
        }
        else{
            reader[counter] = c;
            counter++;
        }
    }
    snd.type = 16;
    snd.num = 0;
    strcpy(snd.text, reader);
    int send = msgget(address, 0666 | IPC_CREAT);
    msgsnd(send, &snd, sizeof(snd), 0);
    msgctl(send, IPC_RMID, NULL);
    close(pd);
    printf("topic list sent\n");
}


int main(int argc, char* argv[]) {
    int msgid = msgget(0x111111, 0666 | IPC_CREAT);
    printf("Message queue ID: %d\n", msgid);
    //generate data structure
    if (mkdir("data", 0777) == -1) {
        fprintf(stderr, "Error creating data folder: %s\n", strerror(errno));
        return 1;
    }
    if (chdir("data") == -1) {
        printf("Error changing to data directory");
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
    close(pd);
    char topics[] = "topics";
    pd = creat(topics, 0777);
    if(pd == -1){
        printf("Error creating list of topics");
        return 1;
    }
    close(pd);
    while(1){
        if (msgrcv(msgid, &rec, sizeof(rec), 0, IPC_NOWAIT) != -1) {
        if(!fork()){
            printf("Received message Type: %ld, Address: %d\n", rec.type, rec.address);
            switch(rec.type){
                case 1:
               	    printf("received login\n");
                    login(rec.top, rec.address);
                    break;
                case 2:
                    printf("received logout\n");
                    logout(rec.top, rec.address);
                    break;
                case 3:
                    printf("received message request\n");
                    addtopic(rec.top, rec.text, rec.address, rec.pro);
                    break;
                case 4:
                    printf("received request to get messages once\n");
                    getmsg(rec.top, rec.pro, rec.address);
                    break;
                case 5:
                    printf("received to request to sub\n");
                    addsub(rec.top, rec.pro);
                    break;
                case 6:
                    printf("received banlist\n");
                    blockuser(rec.top, rec.pro);
                    break;
                case 7:
                    printf("asked for user list\n");
                    givenames(rec.address);
                    break;
                case 8:
                    printf("asked for topic list\n");
                    givetopics(rec.address);
                    break;
                default:
                    senderr(rec.address);
            }
            exit(0);
        }
    }
    }


}