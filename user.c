#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

#define KEY 22342
#define SERVER 1118481
struct msgbuf1 {
        long type;
        char text[256];
        int num;
}rec;

struct msgbuf {
        long type;
        int address;
        int pro; //priority
        char top[50]; //topic or name
        char text[256];
}snd;

void login(char name[], int pid, int where){
    struct msgbuf snd;
    snd.type = 1;
    snd.address = pid;
    strcpy(snd.top, name);
    msgsnd(where, &snd, sizeof(snd), 0);
    printf("Sent message. Type: %ld, Address: %d\n", snd.type, snd.address);
}

void helpinfo() {
    printf("Jak używać:\n"
            "   -Podaj liczbe polecenia\n"
            "   -Jesli jest porzebny dalszy input do polecenia, to zostaniesz poinformowana/y\n"
            "Dostępne polecenia:\n"
            "   -1: Logout()\n"
            "   -2: Napisz wiadomość(temat, treść)\n"
            "   -3: Zasubskrybuj do tematu na stałe(temat)\n"
            "   -4: Podaj ostatnie x wiadomości tematu(temat, x)\n"
            "   -5: Ban user(user)\n"
            "   -6: Podaj listę użytkowników\n"
            "   -7: Podaj listę tematów\n");
}
void handleLogoutMessage() {
    printf("Logout successful.\n");
    int parpid = getppid();
    kill(parpid, SIGTERM);
    exit(0);
}

int main(int argc, char* argv[]){
    if(argc < 2){
        printf("Use: ./program [name]");
        return 1;
    }
    char name[256];
    strcpy(name, argv[1]);
    int me = getpid();
    printf("receiving info from address: %d\n", me);
    me;
    if(SERVER == (me+KEY)){
        printf("Rare error, restart program");
        return 1;
    }
    if(!fork()){ //nasluch
        int msgid = msgget(me+KEY, 0666 | IPC_CREAT);
        while(1){
            msgid = msgget(me+KEY, 0666 | IPC_CREAT);
        	if (msgrcv(msgid, &rec, sizeof(rec), 0, IPC_NOWAIT) != -1) {
                if(!fork()){
                    //printf("RECEIVED A MESSAGE of type: %ld\n", rec.type);
                switch (rec.type) {
                    case 1:
                    printf("success\n");
                    break;
                    case 2:
                    printf("logout error\n");
                    break;
                    case 4:
                    printf("message added to an existing topic\n");
                    break;
                    case 5:
                    printf("message added to a new topic\n");
                    break;
                    case 6:
                    printf("message wasn't added\n");
                    break;
                    case 7:
                    printf("new topic/ban was added\n");
                    break;
                    case 8:
                    printf("topic/ban already exists\n");
                    break;
                    case 9:
                    printf("IDK");
                    break;
                        case 11:
                        printf("Logout successful\n");
                        exit(0);
                        break;
                        case 12:
                        printf("received a message\n");
                        printf("%s", rec.text);
                        break;
                        case 14:
                        printf("topic/ban creation error\n");
                        break;
                        case 15:
                        printf("end of user list");
                        break;
                        case 16:
                        if (rec.num == 1){
                            printf("received topic: %s\n", rec.text);
                        }
                        else{
                            printf("end of topic list: %s\n", rec.text);
                        }
                        case 17:
                        printf("user: %s\n", rec.text);
                        break;
                    default:
                        break;
                        }
                        exit(0);
                }
        	
            }
        }
    }
    else{ //wysylanie
        int sndmsg = msgget(SERVER, 0666 | IPC_CREAT);
        helpinfo();
        login(name, me, sndmsg);
        int n;
        char input[100];
        char inputlong[256];
        char inputshort[50];
        while(1){
            snd.address = me;
            printf("Podaj numer polecenia do wykonania:\n ");
            fgets(input, sizeof(input), stdin);
            sscanf(input, "%d", &n);
            input[strcspn(input, "\n")] = '\0';
            switch(n){
                case 1:
                {
                    printf("Sent the request to logout.\n");
                    snd.type = 2;
                    strcpy(snd.top, name);
                    msgsnd(sndmsg, &snd, sizeof(snd), 0);
                    exit(0);
                    break;
                }
                case 2:
                {
                    snd.type = 3;
                    printf("Podaj temat wiadomosci:\n");
                    fgets(input, sizeof(input), stdin);
                    sscanf(input, "%s", inputshort);
                    printf("Podaj priorytet:\n");
                    fgets(input, sizeof(input), stdin);
                    sscanf(input, "%d", &snd.pro); //dodać sprawdzanie, czy liczba
                    printf("Podaj tytul wiadomosci:\n");
                    fgets(inputlong, sizeof(inputlong), stdin);
                    sscanf(inputlong, "%s", inputlong);
                    printf("Wyslano wiadomosc.\n");
                    strcpy(snd.top, inputshort);
                    strcpy(snd.text, inputlong);
                    msgsnd(sndmsg, &snd, sizeof(snd), 0);
                    break;
                }
                case 3:
                {
                    snd.type = 5;
                    printf("Podaj temat do zasubowania na stale:\n");
                    fgets(input, sizeof(input), stdin);
                    sscanf(input, "%s", inputshort);
                    strcpy(snd.top, inputshort);
                    msgsnd(sndmsg, &snd, sizeof(snd), 0);
                    printf("Wyslano stala subskrypcje.\n");
                    break;

                }
                case 4:
                {
                    snd.type = 4;
                    printf("Podaj temat do otrzymania wiadomosci:\n");
                    fgets(input, sizeof(input), stdin);
                    sscanf(input, "%s", inputshort);
                    printf("Podaj ile wiadomosci chcesz:\n");
                    fgets(input, sizeof(input), stdin);
                    sscanf(input, "%d", &snd.pro);
                    strcpy(snd.top, inputshort);
                    msgsnd(sndmsg, &snd, sizeof(snd), 0);
                    printf("przeslano prosbe o wiadomosc.\n");
                    break;
                }
                case 5:
                {
                    snd.type = 6;
                    printf("Podaj kogo chesz zbanowac:\n");
                    fgets(input, sizeof(input), stdin);
                    sscanf(input, "%s", inputshort);
                    strcpy(snd.top, inputshort);
                    msgsnd(sndmsg, &snd, sizeof(snd), 0);
                    printf("Wyslano prosbe o bana.\n");
                    break;
                }
                case 6:
                {
                    snd.type = 7;
                    msgsnd(sndmsg, &snd, sizeof(snd), 0);
                    break;
                }
                case 7:
                {
                    snd.type = 8;
                    msgsnd(sndmsg, &snd, sizeof(snd), 0);
                    break;

                }
                default:
                {
                    printf("Unkown command.\n");
                    helpinfo();
                    break;
                }
            }
        }

    }
}
