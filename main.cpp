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
    char buffer[100], filename[20];
    char *f;
    int k, size, status, choice;
    int portNumber;
    char serverIP[29];
    int filedescription;
    char key_press;
    int ascii_value;
    bool isUpload = true;
    char key[5];


    if (argc < 3) {
        printf("Error:  The correct format is IP address and port number <54000> \n");
        exit(1);
    }

    //Initialize Socket
    client = socket(AF_INET, SOCK_STREAM, 0);
    if (client < 0) {
        cout << "Socket Creation error" << endl;
        exit(1);
    }

    portNumber = strtol(argv[2], NULL, 10);
    strcpy(serverIP, argv[1]);

    //create socket
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(portNumber);
    server_address.sin_addr.s_addr = inet_addr(serverIP);

    //Connect to server
    k = connect(client, (struct sockaddr *) &server_address, sizeof(struct sockaddr_in));
    if (k < 0) {
        cout << "Error connecting" << endl;
        exit(1);
    }
    //While loop to keep the connection open
    int i = 1;
    while (1) {
        printf("Enter a choice: \n1 - Download\n2 - Upload\n3 - Catalog\n4 - Quit\n");
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                printf("Enter filename to download: ");
                scanf("%s", filename);
                strcpy(buffer, "download ");
                strcat(buffer, filename);
                //Downloads a file from the server
                send(client, buffer, 100, 0);
                recv(client, &size, sizeof(int), 0);
                if (!size) {
                    printf("No such file on the remote directory\n\n");
                    break;
                }
                f = (char *) malloc(size);
                recv(client, f, size, 0);
                while (1) {
                    filedescription = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
                    if (filedescription == -1) {
                        sprintf(filename + strlen(filename), "%d", i);
                    } else {
                        break;
                    }
                }
                write(filedescription, f, size);
                cout <<  "File was successfully downloaded" << endl << "Received: " << size << " bytes" << endl;
                close(filedescription);
                cout << "Press C to display the contents of the file or ESC to continue " << endl;
                while (1) {
                    key_press = fgetc(stdin);
                    ascii_value = key_press;
                    if (ascii_value == 67) {
                        strcpy(buffer, "cat ");
                        strcat(buffer, filename);
                        system(buffer);
                        cout << "Press ESC to continue " << endl;
                    }else if (ascii_value == 99) {
                        strcpy(buffer, "cat ");
                        strcat(buffer, filename);
                        system(buffer);
                        cout << "Press ESC to continue " << endl;
                    }else if (ascii_value == 27){
                        break;
                    }
                }
                break;
            case 2:
                strcpy(buffer, "upload");
                send(client, buffer, 100, 0);
                //uploads a file to the server
                //Keep it in a loop in case the file doesn't exist
                //You can re-enter a new file name to upload
                while(1) {
                    printf("Enter file name to upload to server: ");
                    scanf("%s", filename);
                    filedescription = open(filename, O_RDONLY, 777);

                    if (filedescription == -1) {
                        cout << "No such file on the local directory\n\n" << endl;
                    }else{
                        break;
                    }
                }
                strcpy(buffer, filename);
                send(client, buffer, 4096, 0);
                stat(filename, &obj);
                size = obj.st_size;
                send(client, &size, sizeof(int), 0);
                sendfile(client, filedescription, nullptr, size);
                recv(client, &status, sizeof(int), 0);

                if (status) {
                    printf("Upload was successful\n");
                } else {
                    printf("Upload failed...Files did not store on server\n");
                }
                break;
            case 3:

                strcpy(buffer, "catalog");
                send(client, buffer, 100, 0);
                //Outputs the contents of server directory
                recv(client, &size, sizeof(int), 0);
                f = (char *) malloc(size);
                recv(client, f, size, 0);
                filedescription = creat("temp2.txt", O_WRONLY);
                write(filedescription, f, size);
                close(filedescription);
                system("clear");
                printf("The remote directory listing is:\n");
                system("chmod 777 temp2.txt");
                system("cat temp2.txt");
                system("rm temp2.txt");
                cout << "Press ESC key then Enter to continue..." << endl;
                //After displaying the contents of the server director.
                //Clear the screen for a fresh menu to keep things clean
                while (1) {
                    key_press = fgetc(stdin);
                    ascii_value = key_press;
                    if (ascii_value == 27) { //27 is the ASCII code for the escape key
                        system("clear");
                        break;
                    }
                }

                break;

            case 4:
                //Closes the connection and exits the program
                strcpy(buffer, "bye");
                send(client, buffer, 100, 0);
                recv(client, &status, 100, 0);
                if (status) {
                    printf("Server is shutting down....\n");
                    sleep(2);
                    printf("Server is closed\n");
                    close(client);
                    exit(0);
                }
                printf("Server failed to close connection\n");
        }

    }

    return 0;
}
