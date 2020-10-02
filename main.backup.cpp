#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <cstdio>
#include <cstdlib>


using namespace std;

int main(int argc, char *argv[]) {

    int client;
    struct sockaddr_in server_address;
    struct stat obj;
    char buffer[100], command[5], filename[20];
    char *f;
    int k, size, status, choice;
    int portNumber;
    char serverIP[29];
    int filehandle;
    int rc = 0;

    if (argc < 3) {
        printf("Error:  The correct format is IP address and port number to connect to\n");
        exit(1);
    }

    client = socket(AF_INET, SOCK_STREAM, 0);
    if (client < 0) {
        cout << "Socket Creation error" << endl;
        exit(1);
    }

    portNumber = strtol(argv[2], NULL, 10);
    strcpy(serverIP, argv[1]);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(portNumber);
    server_address.sin_addr.s_addr = inet_addr(serverIP);

    k = connect(client, (struct sockaddr *) &server_address, sizeof(struct sockaddr_in));
    if (k < 0) {
        cout << "Error connecting" << endl;
        exit(1);
    }

    //k = recv(client, &buffer, sizeof(buffer), 0);
    //if (k < 0){
    //cout << "Error receiving from server" << endl;
    //exit(1);
    //}else {
    // printf("%s\n", buffer);
    //}

    //strcpy(buffer, "ls");
    //send(client, buffer, 100, 0);
    //recv(client, &size, sizeof(int), 0);

    int i = 1;
    while(1) {
        printf("Enter a choice: \n1 - Download\n2 - Upload\n3 - Catalog\n4 - Quit\n");
        scanf("%d", &choice);
        switch (choice){
            case 1:
                printf("Enter filename to download: ");
                scanf("%s", filename);
                strcpy(buffer, "download ");
                strcat(buffer, filename);
                send(client, buffer, 100, 0);
                recv(client, &size, sizeof(int), 0);
                if(!size){
                    printf("No such file on the remote directory\n\n");
                    break;
                }
                f = (char *) malloc(size);
                recv(client, f, size, 0);
                while(1){
                    filehandle = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
                    if(filehandle == -1){
                        sprintf(filename + strlen(filename), "%d", i);
                    }else{
                        break;
                    }
                }
                write(filehandle, f, size);
                close(filehandle);
                strcpy(buffer, "cat ");
                strcat(buffer, filename);
                system(buffer);
                break;
            case 2:
                strcpy(buffer, "upload");
                send(client, buffer, 100, 0);
                printf("Enter file name to upload to server: ");
                scanf("%s", filename);
                filehandle = open(filename, O_RDONLY, 777);
                if(filehandle == -1){
                    printf("No such file on the local directory\n\n");
                    break;
                }
//                strcpy(buffer, "upload");
//                send(client, buffer, 100, 0);
                //strcat(buffer, filename);
                strcpy(buffer, filename);
                send(client, buffer, 100, 0);
                stat(filename, &obj);
                size = obj.st_size;
                send(client, &size, sizeof(int), 0);
                sendfile(client, filehandle, nullptr, size);
                recv(client, &status, sizeof(int), 0);
                if(status){
                    printf("Upload was successful\n");
                }else {
                    printf("Upload failed...Files did not store on server\n");
                }
                break;
            case 3:

                strcpy(buffer, "catalog");
                send(client, buffer, 100, 0);
                //Code to output contents of server directory
                recv(client, &size, sizeof(int), 0);
                f = (char *) malloc(size);
                recv(client, f, size, 0);
                filehandle = creat("temp2.txt", O_WRONLY);
                write(filehandle, f, size);
                close(filehandle);
                system("clear");
                printf("The remote directory listing is:\n");
                system("chmod 777 temp2.txt");
                system("cat temp2.txt");
                system("rm temp2.txt");
                break;
            case 4:
                strcpy(buffer, "bye");
                send(client, buffer, 100, 0);
                recv(client, &status, 100, 0);
                if(status)
                {
                    printf("Server closed\nQuitting..\n");
                    exit(0);
                }
                printf("Server failed to close connection\n");
        }

        }





    return 0;


}















    /*
    k = recv(client, &buffer, sizeof(buffer), 0);
    if (k < 0){
        cout << "Error receiving from server" << endl;
        exit(1);
    }else {
        printf("%s\n", buffer);
    }
    cout << "Enter a command" << endl;
    cin >> command;
    char n_command[5];
    sscanf(command, "%s", n_command);
    send(client, &n_command, sizeof(n_command), 0);

    recv(client, &size, sizeof(int), 0);

    void *t;
    t = malloc(size);
    recv(client, t, size, 0);
    filehandle = creat("temp.txt", O_WRONLY);
    write(filehandle, t, size);
    close(filehandle);
    printf("The remote directory listing is:\n");
    system("cat temp.txt");
    */


    //char server_response[256];
   // rc = recv(sd, &server_response, sizeof(server_response), 0);
    //char other_respons[256];
    //int recmsg = recv(sd, &other_respons, sizeof(other_respons), 0);
    //printf("The server sent the data: %s\n", server_response);
    //printf("The server sent this message:  %s\n", other_respons);
    //char command[256];
    //cout << "Type in your command: " << endl;
    //cin >> command;
    //int send_data;
    //send_data = write(sd, command, strlen(command));
    //send(sd, command, sizeof(command), 0);



    /*
    if(rc < 0){
        perror("sendto");
        printf("sent %d bytes\n", rc);

    }
    */


