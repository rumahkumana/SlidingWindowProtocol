#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define BUFFSIZE 50

int main()
{
    int sock,size,connect;
    char senddata[50],data[50], temp[50];
    int packetNumber,NFE,i, j, port;
    struct sockaddr_in ser,cli;
    int receivedPacketArr[BUFFSIZE];
    int leftPointer, rightPointer, currentPointer, windowSize;

    printf("\n\n Server Running ...... ");
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("\n Socket Creation Error");
        exit(-1);
    }
    printf("\nEnter the port number and windows size: ");
    scanf("%d %d",&port, &windowSize);
    ser.sin_family = AF_INET;
    ser.sin_port = htons(port);
    ser.sin_addr.s_addr=INADDR_ANY;
    bzero(&(ser.sin_zero),8);
    if(bind(sock,(struct sockaddr *)&ser,sizeof(struct sockaddr)) == -1) {
        perror("\n\t Error in Bind");
        exit(-1);
    }
    if (listen(sock,2)==-1) {
        perror("\n\t Error in Listen");
        exit(-1);
    }
    printf("\n\t Waiting for connection ");
    size=sizeof(struct sockaddr);
    connect=accept(sock,(struct sockaddr *)&cli,&size);
    
    if(connect==-1) {
        perror("\n\t Connection Failed :");
        exit(-1);
    }
    printf("\n\t Connected Successfully");
    printf("\n");
        // get the pocket number from client
    recv(connect,&packetNumber,sizeof(packetNumber),0);
    NFE=packetNumber;
    /* Inisialiasi array received packet */
    for (i = 0; i < 50; i++) {
        receivedPacketArr[i] = 0;
    }
    leftPointer = 0; rightPointer = windowSize-1; // Inisialisasi 
    while(1)
    {
        i=recv(connect,&data,sizeof(data),0);
        data[i]='\0';
        if (strcmp(data,"end")==0) {
            printf("\n\t Finished");
            break;
        }
        if(packetNumber < leftPointer || packetNumber > rightPointer) {
            strcpy(senddata,"Not in window");
            send(connect,&NFE,sizeof(NFE),0);
            send(connect,senddata,strlen(senddata),0);
        }
        else {
            receivedPacketArr[packetNumber] = 1;
            NFE++;
            /* Update left pointer and right pointer */
            j = leftPointer;
            while (receivedPacketArr[j] != 0) {
                j = (j % BUFFSIZE) + 1;  // Frame accepted already
            }
            rightPointer = (rightPointer + (j - leftPointer) < BUFFSIZE) ? 
            rightPointer + (j - leftPointer) : windowSize; 
            leftPointer = j;
            NFE = j;
            printf("\n The packet Number is : %d",packetNumber);
            printf("\n The data is :%s",data);
            printf("\n Leftpointer, rightPointer is : %d %d", leftPointer, rightPointer);
            printf("\n j : %d\n", j);
            for (i = 0; i < BUFFSIZE; i++) {
                if (receivedPacketArr[i] == 1) printf("Packet %d : %d ", i, receivedPacketArr[i]);
            }
            snprintf(senddata,sizeof(senddata), "Frame number %d acknowledged. Send nextdata", packetNumber);
            send(connect,&NFE,sizeof(NFE),0);
            send(connect,senddata,strlen(senddata),0);
        }
        printf("\n The Expected Packet now is: %d \n",NFE);
        recv(connect,&packetNumber,sizeof(packetNumber),0);
    }
    close(connect);
    close(sock);
return 0;
}