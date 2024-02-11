/* ECE435 Homework #2 webserver code */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/stat.h> // Include for stat()

#define BUFFER_SIZE	4096

/* Default port to listen on */
#define DEFAULT_PORT	8080

void send_response_header(int socket_fd, const char *filename);
const char *get_content_type(const char *filename);
void send_404(int socket_fd);

int main(int argc, char **argv) {

	int socket_fd,new_socket_fd;
	struct sockaddr_in server_addr, client_addr;
	int port=DEFAULT_PORT;
	int n,on=1;
	socklen_t client_len;
	char buffer[BUFFER_SIZE];
	int result;

	printf("Starting server on port %d\n",port);

	/* Open a socket to listen on */
	/* AF_INET means an IPv4 connection */
	/* SOCK_STREAM means reliable two-way connection (TCP) */
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd<0) {
		fprintf(stderr,"Error opening socket! %s\n",
			strerror(errno));
		exit(1);
	}

	/* Set SO_REUSEADDR on socket so that we don't get stuck */
	/* in TIME_WAIT state on early exit */
	setsockopt(socket_fd,SOL_SOCKET,SO_REUSEADDR,(char *)&on,sizeof(on));

	/* Set up the server address to listen on */
	/* The memset sets the address to 0.0.0.0 which means */
	/* listen on any interface. */
	memset(&server_addr,0,sizeof(struct sockaddr_in));
	server_addr.sin_family=AF_INET;
	/* Convert the port we want to network byte order */
	server_addr.sin_port=htons(port);

	/* Bind to the port */
	result=bind(socket_fd,
		(struct sockaddr *) &server_addr,
		sizeof(server_addr));
	if (result <0) {
		fprintf(stderr,"Error binding! %s\n", strerror(errno));
		exit(1);
	}

	/* Tell the server we want to listen on the port */
	/* Second argument is backlog, how many pending connections can */
	/* build up.  We arbitrarily pick 5 */
	listen(socket_fd,5);

wait_for_connection:

	/* Call accept to create a new file descriptor for an incoming */
	/* connection.  It takes the oldest one off the queue */
	/* We're blocking so it waits here until a connection happens */
	client_len=sizeof(client_addr);
	new_socket_fd = accept(socket_fd,
			(struct sockaddr *)&client_addr,&client_len);
	if (new_socket_fd<0) {
		fprintf(stderr,"Error accepting! %s\n",strerror(errno));
		exit(1);
	}

	while(1) {
		/* Someone connected!  Let's try to read BUFFER_SIZE-1 bytes */
		memset(buffer,0,BUFFER_SIZE);
		n = read(new_socket_fd,buffer,(BUFFER_SIZE-1));
		if (n==0) {
			fprintf(stderr,"Connection to client lost\n\n");
			break;
		}
		else if (n<0) {
			fprintf(stderr,"Error reading from socket %s\n",
				strerror(errno));
		}

		//Get request
		char *line = strstr(buffer, "GET");
		if(line){
			char filename[256];
			sscanf(line, "GET /%s", filename);

			struct stat file_stat;
			//handle default file
			if(strcmp(filename, "") == 0 || strcmp(filename, "/") == 0){
				strcpy(filename, "index.html");
			}
			printf("requested file: %s\n", filename);

			// Check if the file exists
            if (stat(filename, &file_stat) != 0) {
                send_404(new_socket_fd); // Send 404 if file not found
                continue;
            }

			send_response_header(new_socket_fd, filename);

			FILE *file = fopen(filename, "rb");
			if(file){
				char file_buffer[1024];
				int bytes_read;
				while((bytes_read =fread(file_buffer, 1, sizeof(file_buffer), file))> 0){
					write(new_socket_fd, file_buffer, bytes_read);
				}
				fclose(file);
				
			}else{
				// Handle file not found or error opening file
                send_404(new_socket_fd); // Send 404 in case of fopen failure
			}
		}else{
			printf("No GET request found.\n");//we didn't receive any GET request
		}


		/* Print the message we received */
		printf("Message received: %s\n",buffer);



	}
	close(new_socket_fd);

	printf("Done connection, go back and wait for another\n\n");

	goto wait_for_connection;

	/* Close the sockets */
	close(socket_fd);
	return 0;
}
void send_response_header(int socket_fd, const char *filename) {
    struct stat file_stat;
    char header[1024];
    char date_buffer[30];
    time_t now = time(0);
    struct tm *gmt = gmtime(&now);

    stat(filename, &file_stat);

    strftime(date_buffer, sizeof(date_buffer), "%a, %d %b %Y %H:%M:%S GMT", gmt);

    sprintf(header, 
      	    "HTTP/1.1 200 OK\r\n"
      	    "Date: %s\r\n"
      	    "Server: ECE435\r\n"
     	    "Last-Modified: %s\r\n"
     	    "Content-Length: %ld\r\n"
     	    "Content-Type: %s\r\n\r\n",
            date_buffer, date_buffer, file_stat.st_size, get_content_type(filename));

    	write(socket_fd, header, strlen(header));
}

const char *get_content_type(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (!dot) return "text/plain";
    if (strcmp(dot, ".html") == 0) return "text/html";
    if (strcmp(dot, ".txt") == 0) return "text/plain";
    // Add other content types as needed
    return "text/plain";
}

void send_404(int socket_fd) {
    char *response =
        "HTTP/1.1 404 Not Found\r\n"
        "Date: Fri, 08 Sep 2017 04:56:25 GMT\r\n" // Replace with actual date
        "Server: ECE435\r\n"
        "Content-Length: 100\r\n"
        "Connection: close\r\n"
        "Content-Type: text/html;charset=iso-8859-1\r\n\r\n"
        "<html><head>\r\n"
        "<title>404 Not Found</title>\r\n"
        "</head><body>\r\n"
        "<h1>Not Found</h1>\r\n"
        "<p>The requested URL was not found on this server.<br />\r\n"
        "</p>\r\n"
        "</body></html>\r\n";

    write(socket_fd, response, strlen(response));
}


