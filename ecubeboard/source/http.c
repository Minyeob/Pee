#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include "http.h"

#define IPADDR	"52.78.209.143"
#define PORT	9000


// connect
int httpConnect()
{
	struct sockaddr_in sockaddr;
	if((fd[10] =socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		return 0;
	}
	//memset(sockaddr, 0, sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = inet_addr(IPADDR);
	sockaddr.sin_port = htons(PORT);
	
	if(connect(fd[10], (struct sockaddr*)&sockaddr, sizeof(struct sockaddr)) < 0)
	{
		return 0;
	}
	
	return 1;
}


 
// Get the content length.
int getContentLength(char* str)
{
	char* pch = strstr(str, "Content-Length:" );
	
	int number = 0;
	int i;

	for(i = 15 ; i < strlen(pch) ; ++i)
	{   
		if( pch[i] <= '9' && pch[i] >= '0')
		{   
			number *=  10; 
			number += (pch[i] - '0');
		}   

		if(*(pch+i) == '\r') break;

	}   
	return number;
}


void httpClose()
{
	close(fd[10]);
}

// Check and return the user.
char* request(char* method, char *page, char* data)
{
	char query[1024];
	char buf[16384];
	int sendt = 0;
	int buffLen = 0;
	int len;
	int remain;
	int contentLength;

	char* contentPoint;
	char* content;	

	// httpConnect
	httpConnect();


	memset(query,0,sizeof(query));
	//request

	sprintf(query,"%s %s HTTP/1.1\r\nHost: %s\r\n",method, page, IPADDR);

	//http POST
	if(!strcmp(method, "POST"))
	{   
		strcat(query, "content-type: application/json\r\n");
		sprintf(query+strlen(query), "content-length: %ld\r\n\r\n", strlen(data));
		strcat(query, data);
	}   
	else //http GET
	{   
		strcat(query, "\r\n");
	}   


	while(sendt < strlen(query))
	{
		if((len = send(fd[10], query + sendt, strlen(query) - sendt, 0)) < 0)
		{
			return 0;
		}
		sendt += len;
	}


	memset(buf, 0, sizeof(buf));


	// response header
	while((len = recv(fd[10], query, 1024, 0)) > 0)
	{
		strncpy(buf + buffLen, query, len);
		buffLen += len;
		
		contentPoint = strstr(buf, "\r\n\r\n");
		
		if( contentPoint ) break;

	}

	contentPoint += 4;
	contentLength = getContentLength(buf);

	remain = (buffLen - (int)(contentPoint - buf)) - contentLength;
	
	// remain response content
	if(remain != 0)
	{
		while((len = recv(fd[10], query, 1024, 0)) > 0)
		{
			strncpy(buf + buffLen, query, len);
			buffLen += len;
			if(len - remain >= 0) break;	
		}
	}

	len = 0;	
	buf[buffLen] = 0;	
	content = (char*)malloc(strlen(contentPoint));
	
	strcpy(content, contentPoint);
	httpClose(); //http close

	if(!strcmp(content,"[]"))
		strcpy(content,"");

	return content;
}

