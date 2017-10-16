#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define SEGSIZE 8

/************* SEGMENT CONSTRUCTING FUNCTIONS ***************/
/* A utility function to reverse a string  */
void reverse(char * str, int length)
{
    int start = 0;
    int end = length -1;
    char temp;

    while (start < end)
    {
        temp = *(str+start);
        *(str+start) = *(str+end);
        *(str+end) = temp;
        start++;
        end--;
    }
}

// Implementation of itoa()
char* itoa(int num, char* str, int base)
{
    int i = 0;
    int isNegative;
    
    isNegative = 0;
    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
    // In standard itoa(), negative numbers are handled only with 
    // base 10. Otherwise numbers are considered unsigned.
    if (num < 0 && base == 10)
    {
        isNegative = 1;
        num = -num;
    }
    // Process individual digits
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        num = num/base;
    }
    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';
    str[i] = '\0'; // Append string terminator
    // Reverse the string
    reverse(str, i);
 
    return str;
}

int construct(char * segment, int seqNum, char * data) {
    char seqTemp[4]; //buffer for seqNum conversion 
    int i;

    itoa(seqNum, seqTemp, 10);
    segment[0] = 0x1;
    for (i = 1; i < 5; i++) {
        segment[i] = seqTemp[i-1];
    }
    segment[5] = 0x2;
    segment[6] = data[0];
    segment[7] = 0x3;
    segment[8] = data[0];
}

/****************************************************************/

int main()
{
        int sock,val,i,count,port;
        char recvdata[50],sentdata[50];
        struct sockaddr_in server_addr;
        char segment[SEGSIZE];

        printf("\n\n Client Running ...... ");
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
            perror("Socket");
            exit(1);
        }
        printf("\nEnter the port number");
        scanf("%d",&port);
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        server_addr.sin_addr.s_addr= htonl(INADDR_ANY);
        bzero(&(server_addr.sin_zero),8);
        if (connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
        {
            perror("Connect");
            exit(1);
        }
        while(1)
        {
                //get the pack number from client
                printf("\n Enter packet number ");
                scanf("%d",&val);
                // sent the value to server
                send(sock,&val,sizeof(val),0);
                // get the data from the user
                printf("\n\n Enter data ");
                scanf("%s",sentdata);
                // sent the to server
                send(sock,sentdata,strlen(sentdata),0);
                   if(strcmp(sentdata,"end")==0)
                   break;
                // recev the result from server
                recv(sock,&count,sizeof(count),0);
                i=recv(sock,recvdata,50,0);
                recvdata[i]='\0';
                printf("\n %s %d",recvdata,count);
        }
        close(sock);
        return 0;
}
