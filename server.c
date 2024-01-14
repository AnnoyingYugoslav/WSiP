#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>


bool serch(char text[50], int file){
    switch(file){
        case 1: //patrz, czy jest temat
            //do
            break;
        case 2: //patrz, czy jest użytkownik
            //do
            break;
        default:
            return NULL;
    }
}

void login(char name[50], int address){
    if(!search(name, 2)){
        //make new file banname
        //make new file subname - first is the adress
        snd.type = 1;
        int i;
        int counter = 0;
        char topic[50];
        i = //ilość tematów;
        for(int j = 0; j < i; j++){
            //get the j-th topic to topic
            int size = (sizeof(topic) / sizeof(char));
            for(int a = 0; a < size; a++){
                snd.text[counter] = topic[a];
                counter++;
            }
            snd.text[counter] = ';';
            counter++;
        }
    }
    else{
        snd.type = 2;
        snd.text[0] = '';
    }
    msgsnd(address, &snd, sizeof(snd), 0);
    msgctl(address, IPC_RMID, NULL);
}

void logout(){

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
        int num; //how many messeges left - if multi message
    }
    struct msgbuf2 rec {
        long type;
        int user; //signal how to talk back
        char top[50]; //topic or name
        char text[256];
    }

    int msgid = msgget(0x111111, 0666 | IPC_CREAT);

    while(1){
        msgrcv(msgid, &rec, sizeof(rec) - sizeof(long), 0, 0);
        if(!fork()){
            switch(rec.type){
                case 1:
                    login(rec.top, rec.user);
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