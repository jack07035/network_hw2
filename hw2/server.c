#include "net.h"
#include <ctype.h>

#define MAX_REQUEST_SIZE 4096

char martrix[10];

struct player
{
    char name[20];
    char password[20];
    int online ;
    int user;
    int infight;
    int parner;
    int turn;
    int ox;
    int see[20];
    int board [20][3];
    char martrix[10];
};

struct player player[20];

int playernum=0;

void setaccount(FILE *fp)
{
    while(fscanf(fp,"%s",player[playernum].name)!=EOF)
    {
        fscanf(fp,"%s",player[playernum].password);
        player[playernum].online = 0;
        playernum++;
    }
    /*strcpy(player[0].name,"jack");
    strcpy(player[1].name,"occdd");
    strcpy(player[2].name,"eleven");
    strcpy(player[3].name,"lena");
    strcpy(player[4].name,"peter");

    strcpy(player[0].password,"jack");
    strcpy(player[1].password,"occdd");
    strcpy(player[2].password,"eleven");
    strcpy(player[3].password,"lena");
    strcpy(player[4].password,"peter");

    for(int i=0;i<5;i++)
    {
        player[i].online = 0 ;
    }*/
}

void signup(int i)
{
    char name[100],password[100];
    memset(name,'\0',100);memset(password,'\0',100);


    send(i, "Please enter account you want to creat:\n\0", strlen("Please enter account you want to creat:\n\0"),0);
    recv(i,name,sizeof(name),0);

    send(i, "Please enter password tou want to creat:\n", strlen("Please enter password tou want to creat:\n"),0);
    recv(i,password,sizeof(password),0);
    strcpy(player[playernum].name,name);
    strcpy(player[playernum].password,password);
    player[playernum].user = i;
    player[playernum].online = 1;
    player[playernum].infight = 0;
    player[playernum].parner = -1;
    memset(player[playernum].see,0,20);
    for(int n;n<playernum+1;n++)
    {
        for(int m;m<3;m++)
        {
            player[playernum].board[n][m]=0;
            player[n].board[playernum][m]=0;
        }
    }
    playernum++;
    send(i,"Signup sussece ! Welcome\nYou can type help to see what to do\n", strlen("Signup sussece ! Welcome\nYou can type help to see what to do\n"),0);
    FILE *fp;
    fp = fopen("data.txt","a+");
    fprintf(fp,"%s\n%s\n",name,password);
    fclose(fp);
}

void login(int i)
{
    int flag=0;
    while(flag==0)
    {
        char name[100],password[100];
        memset(name,'\0',100);memset(password,'\0',100);

        send(i, "Please enter your account :\n\0", strlen("Please enter your account :\n\0"),0);
        recv(i,name,sizeof(name),0);
        
        send(i, "Please enter your password :\n", strlen("Please enter your password :\n"),0);
        recv(i,password,sizeof(password),0);


        for(int j=0;j<playernum;j++)
        {
            if(strcmp(player[j].name,name)==0&&strcmp(player[j].password,password)==0)
            {
                if(player[j].online == 1)
                {
                    send(i,"This account is already login ! please login again .\n", strlen("This account is already login ! please login again .\n"),0);
                    break;
                }
                flag=1;
                player[j].user = i;
                player[j].online = 1;
                player[j].infight = 0;
                player[j].parner = -1;
                memset(player[j].see,0,20);
                for(int n;n<playernum;n++)
                {
                    for(int m;m<3;m++)
                    {
                        player[j].board[n][m]=0;
                    }
                }
            }
        }
        if(flag==0)
        {
            send(i,"Login fail ! Please login again .\n", strlen("Login fail ! please login again .\n"),0);
        }
        else
        {
            send(i,"Login sussece ! Welcome\nYou can type help to see what to do\n", strlen("Login sussece ! Welcome\nYou can type help to see what to do\n"),0);
        }
    }
}

void list(int n)
{
    for(int i = 0;i<playernum;i++)
    {
        char read[1000];
        memset(read,'\0',sizeof(read));
        if(player[i].online==1)
        {
            sprintf(read,"%s\t\t\ton\n",player[i].name);
            send(n,read,strlen(read),0);
        }
        else
        {
            sprintf(read,"%s\t\t\toff\n",player[i].name);
            send(n,read,strlen(read),0);
        }   
    }
}

char *findname(int n)
{
    for(int i=0;i<playernum;i++)
    {
        if(player[i].user==n)
        return player[i].name;
    }
    return NULL;
}

int finduser(char *name)
{
    for(int j=0;j<playernum;j++)
    {
        if(strcmp(name,player[j].name)==0)
        {
            return player[j].user;
        }
    }
    return -1;
}

int findplayer(int n)
{
    for(int i=0;i<playernum;i++)
    {
        if(player[i].user==n)
        return i;
    }
    return -1;
}

int challenge(int i,int j)
{
    send(i,"Waiting response ....\n",strlen("Waiting response ....\n"),0);

    char *cname,read[1000];
    memset(read,'\0',sizeof(read));
    cname=findname(i);

    sprintf(read,"%s want to challenge you !\n(type yes/no)\n",cname);
    
    send(j,read,strlen(read),0);
    memset(read,'\0',sizeof(read));
    recv(j,read,sizeof(read),0);
    printf("res %s\n",read);
    if(strcmp(read,"yes")==0)
    {
        send(i,"Accept !\n",strlen("Accept !\n"),0);
        player[findplayer(i)].infight = 1;
        player[findplayer(j)].infight = 1;
        player[findplayer(i)].parner =findplayer(j) ;
        player[findplayer(j)].parner =findplayer(i) ;
        player[findplayer(i)].turn = 1;
        player[findplayer(j)].turn = 0;
        player[findplayer(i)].ox = 1;
        player[findplayer(j)].ox = 0;
        memset(player[findplayer(i)].martrix,' ',10);
        memset(player[findplayer(j)].martrix,' ',10);
        char line[100];
        memset(line,'\0',100);
        strcpy(line,"Game start !\n");
        send(i,line,strlen(line),0);
        send(j,line,strlen(line),0);
        //memset(martrix,' ',10);

        char game[100];
        memset(game,'\0',100);
        sprintf(game," 7| 8| 9 \n一 一 一 \n 4| 5| 6 \n一 一 一 \n 1| 2| 3 \n");
        send(i,game,strlen(game),0);

        send(j,game,strlen(game),0);

        send(i,"It is your turn !\n",strlen("It is your turn !\n"),0);
        send(j,"Waiting your opponent ...\n",strlen("Waiting your opponent ...\n"),0);
        for(int s=0;s<playernum;s++)
        {
            if(player[s].see[findplayer(i)]==1||player[s].see[findplayer(j)]==1)
            {
                char line[1000];
                memset(line,'\0',sizeof(line));
                sprintf(line,"Now is %s's turn !\n",findname(i));
                send(player[s].user,line,strlen(line),0);
            }
        }
    }
    else
    {
        send(i,"Refuse !\n",strlen("Refuse !\n"),0);
    }
}

int win(int n)
{
    if((player[n].martrix[1]==player[n].martrix[2]&&player[n].martrix[2]==player[n].martrix[3]&&player[n].martrix[3]!=' ')||(player[n].martrix[4]==player[n].martrix[5]&&player[n].martrix[5]==player[n].martrix[6]&&player[n].martrix[6]!=' ')||(player[n].martrix[7]==player[n].martrix[8]&&player[n].martrix[8]==player[n].martrix[9]&&player[n].martrix[9]!=' '))
    {
        return 1;
    }
    else if((player[n].martrix[1]==player[n].martrix[4]&&player[n].martrix[4]==player[n].martrix[7]&&player[n].martrix[7]!=' ')||(player[n].martrix[2]==player[n].martrix[5]&&player[n].martrix[5]==player[n].martrix[8]&&player[n].martrix[8]!=' ')||(player[n].martrix[3]==player[n].martrix[6]&&player[n].martrix[6]==player[n].martrix[9]&&player[n].martrix[9]!=' '))
    {
        return 1;
    }
    else if((player[n].martrix[1]==player[n].martrix[5]&&player[n].martrix[5]==player[n].martrix[9]&&player[n].martrix[9
    ]!=' ')||(player[n].martrix[3]==player[n].martrix[5]&&player[n].martrix[5]==player[n].martrix[7]&&player[n].martrix[7]!=' '))
    {
        return 1;
    }
    else if(player[n].martrix[1]!=' '&&player[n].martrix[2]!=' '&&player[n].martrix[3]!=' '&&player[n].martrix[4]!=' '&&player[n].martrix[5]!=' '&&player[n].martrix[6]!=' '&&player[n].martrix[7]!=' '&&player[n].martrix[8]!=' '&&player[n].martrix[9]!=' ')
    {
        return 2;
    }
    else
    {
        return 0;
    }
    
}

void reset(int n)
{
    player[findplayer(n)].infight = 0;
    player[player[findplayer(n)].parner].infight= 0;
    memset(player[findplayer(n)].martrix,' ',10);
    memset(player[player[findplayer(n)].parner].martrix,' ',10);
}

int ooxx(int n,int point)
{
    int result;
    if(player[findplayer(n)].martrix[point]!=' ')
    {
        send(n,"This place had been selected!\nPlease select another !\n",strlen("This place had been selected!\nPlease select another !\n"),0);
        return -1;
    }
    if(player[findplayer(n)].ox==1)
    {
        player[findplayer(n)].martrix[point]= 'O';
        player[player[findplayer(n)].parner].martrix[point]='O';
    }
    else
    {
        player[findplayer(n)].martrix[point]= 'X';
        player[player[findplayer(n)].parner].martrix[point]='X';
    }
    char game[100];
    sprintf(game," %c| %c| %c \n一 一 一 \n %c| %c| %c \n一 一 一 \n %c| %c| %c \n",player[findplayer(n)].martrix[7],player[findplayer(n)].martrix[8],player[findplayer(n)].martrix[9],player[findplayer(n)].martrix[4],player[findplayer(n)].martrix[5],player[findplayer(n)].martrix[6],player[findplayer(n)].martrix[1],player[findplayer(n)].martrix[2],player[findplayer(n)].martrix[3]);
    send(n,game,strlen(game),0);
    send(n,"\n",strlen("\n"),0);
    send(player[player[findplayer(n)].parner].user,game,strlen(game),0);
    send(player[player[findplayer(n)].parner].user,"\n",strlen("\n"),0);
    for(int s=0;s<playernum;s++)
    {
        if(player[s].see[findplayer(n)]==1||player[s].see[player[findplayer(n)].parner]==1)
        {
            char line[1000];
            memset(line,'\0',sizeof(line));
            sprintf(line,"%s\n",game);
            send(player[s].user,line,strlen(line),0);
        }
    }

    result =win(findplayer(n));

    if(result==1)
    {
        send(n,"Congratulation you win !\n",strlen("Congratulation you win !\n"),0);
        send(player[player[findplayer(n)].parner].user,"Sorry you lose !\n",strlen("Sorry you lose !\n"),0);
        reset(n);
        for(int s=0;s<playernum;s++)
        {
            if(player[s].see[findplayer(n)]==1||player[s].see[player[findplayer(n)].parner]==1)
            {
                char line[1000];
                memset(line,'\0',sizeof(line));
                sprintf(line,"%s win !\n",findname(n));
                send(player[s].user,line,strlen(line),0);
            }
        }
        player[findplayer(n)].board[player[findplayer(n)].parner][0]++;
        player[player[findplayer(n)].parner].board[findplayer(n)][1]++;
    }
    else if(result ==2)
    {
        send(n,"You two draw !\n",strlen("You two draw !\n"),0);
        send(player[player[findplayer(n)].parner].user,"You two draw\n",strlen("You two draw\n"),0);
        reset(n);
        for(int s=0;s<playernum;s++)
        {
            if(player[s].see[findplayer(n)]==1||player[s].see[player[findplayer(n)].parner]==1)
            {
                send(player[s].user,"Draw !",strlen("Draw !"),0);
            }
        }
        player[findplayer(n)].board[player[findplayer(n)].parner][2]++;
        player[player[findplayer(n)].parner].board[findplayer(n)][2]++;
    }
    else
    {
        player[findplayer(n)].turn = 0;
        player[player[findplayer(n)].parner].turn = 1;
        send(player[player[findplayer(n)].parner].user,"It is your turn !\n",strlen("It is your turn !\n"),0);
        send(n,"Waiting your opponent ...\n",strlen("Waiting your opponent ...\n"),0);
        for(int s=0;s<playernum;s++)
        {
            if(player[s].see[findplayer(n)]==1||player[s].see[player[findplayer(n)].parner]==1)
            {
                char line[1000];
                memset(line,'\0',sizeof(line));
                sprintf(line,"Now is %s's turn !\n",findname(player[player[findplayer(n)].parner].user));
                send(player[s].user,line,strlen(line),0);
            }
        }
    }
    
}

void logout(int n)
{
    send(n,"Are you sure to logout ?\n(yes/no)\n",strlen("Are you sure to logout ?\n(yes/no)\n"),0);
    char read[1000];
    memset(read,'\0',1000);
    recv(n,read,sizeof(read),0);
    if(strcmp(read,"yes")==0)
    {
        player[findplayer(n)].online = 0;

        send(n,"Tanks for your playing !\nSee you next time\n",strlen("Tanks for your playing !\nSee you next time\n"),0);
    }
}

void board(int n)
{
    for(int i=0;i<playernum;i++)
    {
        if(i == findplayer(n))
        {
            continue;
        }
        char read[1000];
        memset(read,'\0',1000);
        sprintf(read,"%s    \t%d win\t%d loss\t%d draw\n",player[i].name,player[findplayer(n)].board[i][0],player[findplayer(n)].board[i][1],player[findplayer(n)].board[i][2]);
        send(n,read,strlen(read),0);
    }
}

void wechat(int n,int k ,char *chat)
{
    char line[1200];
    memset(line,'\0',sizeof(line));
    sprintf(line,"%s say : %s\n",findname(n),chat);

    send(k,line,strlen(line),0);
}

void help(int n)
{
    send(n,"You can type these commant :\n",strlen("You can type these commant .\n"),0);
    send(n,"list      \t(list out who is online)\n",strlen("list      \t(list out who is online)\n"),0);
    send(n,"challenge \t(to play with others)\n",strlen("challenge \t(to play with others)\n"),0);
    send(n,"send      \t(to chat with others)\n",strlen("send      \t(to chat with others)\n"),0);
    send(n,"see       \t(to watch others playing)\n",strlen("see       \t(to watch others playing)\n"),0);
    send(n,"see off   \t(to quit watch others playing)\n",strlen("see off   \t(to quit watch others playing)\n"),0);
    send(n,"board     \t(to see your score)\n",strlen("board     \t(to see your score)\n"),0);
    send(n,"logout    \t(logout)\n",strlen("logout    \t(logout)\n"),0);
    send(n,"quit    \t(disconnect)\n",strlen("quit    \t(disconnect)\n"),0);
}

int main() {

    printf("Configuring local address...\n");
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *bind_address;
    getaddrinfo(0, "8080", &hints, &bind_address);


    printf("Creating socket...\n");
    SOCKET socket_listen;
    socket_listen = socket(bind_address->ai_family,
            bind_address->ai_socktype, bind_address->ai_protocol);
    if (!ISVALIDSOCKET(socket_listen)) {
        fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }


    printf("Binding socket to local address...\n");
    if (bind(socket_listen,
                bind_address->ai_addr, bind_address->ai_addrlen)) {
        fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
    freeaddrinfo(bind_address);


    printf("Listening...\n");
    if (listen(socket_listen, 10) < 0) {
        fprintf(stderr, "listen() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    fd_set master;
    FD_ZERO(&master);
    FD_SET(socket_listen, &master);
    SOCKET max_socket = socket_listen;

    printf("Waiting for connections...\n");


    FILE *fp;
    fp = fopen("data.txt","a+");
    setaccount(fp);
    fclose(fp);
    //printf("%d\n",playernum);

    while(1) {
        fd_set reads;
        reads = master;
        if (select(max_socket+1, &reads, 0, 0, 0) < 0) {
            fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO());
            return 1;
        }

        SOCKET i;
        int waitlog[100] ;
        memset(waitlog,0,100);
        for(i = 1; i <= max_socket; ++i) {

            if (FD_ISSET(i, &reads)) {
                //printf("%d iiii\n",i);

                if (i == socket_listen) {//printf("hi\n");
                    struct sockaddr_storage client_address;
                    socklen_t client_len = sizeof(client_address);
                    SOCKET socket_client = accept(socket_listen,
                            (struct sockaddr*) &client_address,
                            &client_len);
                    if (!ISVALIDSOCKET(socket_client)) {
                        fprintf(stderr, "accept() failed. (%d)\n",
                                GETSOCKETERRNO());
                        return 1;
                    }

                    FD_SET(socket_client, &master);
                    if (socket_client > max_socket)
                        max_socket = socket_client;

                    char address_buffer[100];
                    getnameinfo((struct sockaddr*)&client_address,
                            client_len,
                            address_buffer, sizeof(address_buffer), 0, 0,
                            NI_NUMERICHOST);
                    printf("New connection from %s\n", address_buffer);

                    send(socket_client,"You can login or signup\n",strlen("You can login or signup\n"),0);

                } else {
                    char read[1024];
                    memset(read,'\0',sizeof(read));
                    int bytes_received = recv(i, read, 1024, 0);
                    if (bytes_received < 1) {
                        FD_CLR(i, &master);
                        CLOSESOCKET(i);
                        continue;
                    }
                    if(strcmp(read,"list")==0)
                    {
                        list(i);
                    }
                    else if(strcmp(read,"login")==0)
                    {
                        login(i);
                    }
                    else if(strcmp(read,"signup")==0)
                    {
                        signup(i);
                    }
                    else if(strncmp(read,"send",4)==0)
                    {
                        char chat[1000],person[100],*ptr,*qtr;
                        memset(chat,'\0',1000);
                        memset(person,'\0',100);
                        ptr = read ;
                        if(ptr = strstr(ptr," "))
                        {
                            ptr=ptr+1;
                            qtr = person;
                            while((*ptr)!=' '&&(*ptr)!='\0')
                            {
                                *qtr = *ptr;
                                qtr++;
                                ptr++;
                            }
                            *qtr= '\0';
                            ptr++;
                            qtr = chat;
                            while((*ptr)!='\n'&&(*ptr)!='\0')
                            {
                                *qtr = *ptr;
                                qtr++;
                                ptr++;
                            }
                            *qtr= '\0';
                        }
                        
                        int to=0;
                        to = finduser(person);
                        if(strlen(chat)!=0&&to!=-1)
                        {
                            //printf("%d,,,%s...\n",to,chat);
                            wechat(i,to,chat);
                        }
                    }
                    else if(strcmp(read,"see off")==0)
                    {
                        memset(player[findplayer(i)].see,0,20);
                        //printf("%d%d%d%d%d\n",player[findplayer(i)].see[0],player[findplayer(i)].see[1],player[findplayer(i)].see[2],player[findplayer(i)].see[3],player[findplayer(i)].see[4]);
                    }
                    else if(strncmp(read,"see",3)==0)
                    {
                        char seek[100],*ptr,*qtr;
                        memset(seek,'\0',100);
                        ptr=read;
                        qtr=seek;
                        if((ptr=strstr(read," ")))
                        {
                            ptr++;
                            while((*ptr)!='\0'&&(*ptr)!='\n'&&(*ptr)!=' ')
                            {
                                *qtr=*ptr;
                                qtr++;
                                ptr++;
                            }
                            *qtr='\0';
                            int yo=0;
                            yo=finduser(seek);
                            if(strlen(seek)!=0&&yo!=-1)
                            {
                                player[findplayer(i)].see[findplayer(yo)] =1;
                            }
                        }
                    }
                    else if(player[findplayer(i)].infight==1)
                    {
                        if(player[findplayer(i)].turn == 0)
                        {
                            send(i,"It is not your turn !\n",strlen("It is not your turn !\n"),0);
                        }
                        else
                        {
                            int point ;
                            char number;
                            number = read[0];
                            if(number>'9'||number<'1')
                            {
                                send(i,"Please enter 0~9\n",strlen("Please enter 0~9\n"),0);
                            }
                            point = atoi(read);
                            ooxx(i,point);
                        }
                    }
                    else if(strncmp(read,"challenge",9)==0)
                    {
                        int j;
                        char *ptr,*qtr,word[100];
                        memset(word,'\0',100);
                        if(!(ptr=strstr(read," ")))
                        {
                            continue;
                        }
                        ptr=ptr+1;
                        qtr=word;
                        while((*ptr)!='\0'&&(*ptr)!='\n')
                        {
                            *qtr=*ptr;
                            qtr++;
                            ptr++;
                        }
                        *qtr='\0';

                        //printf("cccc %d %d %s\n",i,finduser(word),word);

                        challenge(i,finduser(word));
                    }
                    else if(strcmp(read,"logout")==0)
                    {
                        logout(i);
                    }
                    else if(strcmp(read,"board")==0)
                    {
                        //printf("sssss\n");
                        board(i);
                    }
                    else if(strcmp(read,"help")==0)
                    {
                        help(i);
                    }
                    else if(strcmp(read,"quit")==0)
                    {
                        send(i,"quit\n",strlen("quit\n"),0);
                    }
                    
                }              
            } //if FD_ISSET
        } //for i to max_socket
    } //while(1)

    printf("Closing listening socket...\n");
    CLOSESOCKET(socket_listen);


#if defined(_WIN32)
    WSACleanup();
#endif


    printf("Finished.\n");

    return 0;
}