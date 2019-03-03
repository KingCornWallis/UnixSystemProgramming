#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

using namespace std;

int main(int argc, char **argv)
{
     int sockfd, port, len;
    struct addrinfo h, *r;
    int result, nread;
    string inputChoice, response;
    
    string score1, score2;

    string inputTxt, outputTxt, processTxt, inputFileName, outputFileName; 

    static char buffer[128];
    int buflen = sizeof(buffer);

    if (argc != 3) 
    {
        cerr << "Usage: " << argv[0] << " SRC\n";
        return 1;
    }
    getaddrinfo(argv[1], argv[2], NULL, &r);
    sockfd = socket(r->ai_family, r->ai_socktype, r->ai_protocol);
    result = connect(sockfd, r->ai_addr, r->ai_addrlen);
    if(result == -1) 
    {
        cerr << "problem with client" << endl;
        exit(1);
    }
    
    
    while (true)
    {
      
        write(sockfd, inputChoice.c_str(), inputChoice.length() + 1);
        int ready = 0;
        cout << inputChoice;
        nread = read(sockfd, buffer, buflen);
        if (inputChoice.length() == 0)
        {
            cout << "exiting" << endl;
            break;
        }
        if (strcmp(buffer, "STOP") == 0)
        {

            break;
        }
        else if (strcmp(buffer, "GO") == 0)
            ready = 1;

        nread = read(sockfd, buffer, buflen);
        if (strcmp(buffer, "1") == 0)
            cout << "You are Player 1." << endl;
        else
            cout << "You are Player 2." << endl;
        if (ready)
        {
            while (1)
            {
                cout << "0: Exit" << endl;
                cout << "1: Rock" << endl;
                cout << "2: Paper" << endl;
                cout << "3: Scissors" << endl;
                cout << "Enter Choice: " << endl;
                cin >> inputChoice;

                int choice = atoi(inputChoice.c_str());
                if (choice < 0 || choice > 3)
                {
                    cout << "Please only enter numbers 0-3; you entered: " << inputChoice << endl;
                    continue;
                }

                write(sockfd, inputChoice.c_str(), inputChoice.length() + 1);

                nread = read(sockfd, buffer, buflen);

                if (strcmp(buffer, "STOP") == 0)
                {
                    nread = read(sockfd, buffer, buflen);
                    score1 = buffer;
                    nread = read(sockfd, buffer, buflen);
                    score2 = buffer;
                    cout << "Game has ended" << endl;
                    cout << "Final Score" << endl;
                    cout << "Player 1: " << score1 << endl;
                    cout << "Player 2: " << score2 << endl;

                    write(sockfd, inputChoice.c_str(), inputTxt.length() + 1);

                    nread = read(sockfd, buffer, buflen);
                    //outFile << buffer << endl;
                    
                    return 0;
                }
                cout << "Message Sent" << endl;
            }
        }
    }
    close(sockfd);
    exit(0);
}
