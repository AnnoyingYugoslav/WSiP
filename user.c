#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#define KEY 22342
#define SERVER 1118481

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

struct msgbuf1 {
        long type;
        char text[256];
        int num;
}rec;

struct msgbuf {
        long type;
        int address;
        int pro; //prioriyty
        char top[50]; //topic or name
        char text[256];
}snd;

int main(int argc, char* argv[]){
    if(argc < 2){
        printf("Use: ./program [name]");
        return 1;
    }
    char name[] = argv[1];
    int me = getpid();
    me;
    if(SERVER == (me+KEY)){
        printf("Rare error, restart program");
        return 1;
    }
    //zaloguj sie



    //test if zalogowano

    if(fork()){ //nasluch
        int msgid = msgget(me, 0666 | IPC_CREAT);
        while(1){

        }
    }
    else{ //wysylanie
        int sndmsg = msgget(SERVER, 0666 | IPC_CREAT);
        helpinfo();
        login(name, me);
        int n;
        char input[100];
        char inputlong[256];
        char inputshort[50];
        while(1){
            printf("Podaj numer polecenia do wykonania: ");
            fgets(input, sizeof(input), stdin);
            sscanf(input, "%d", &n);
            switch(n){
                case 1:
                {
                    snd.type = 2;
                }
                case 2:
                {
                    snd.type = 3;
                }
                case 3:
                {
                    snd.type = 5;
                }
                case 4:
                {
                    snd.type = 4;
                }
                case 5:
                {
                    snd.type = 6;
                }
                case 6:
                {
                    snd.type = 7;
                    msgsnd(sndmsg, &snd, sizeof(snd), 0);
                }
                case 7:
                {
                    snd.type = 8;
                    msgsnd(sndmsg, &snd, sizeof(snd), 0);

                }
                default:
                {
                    printf("Unkown command.\n");
                    helpinfo();
                }
            }
        }

    }
}