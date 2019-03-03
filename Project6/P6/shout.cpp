#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
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
    string input, response;
    
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
        cerr << "client: connect failed" << endl;
        exit(1);
    }
    
    while(true) 
    {
        cout << "Enter input and output file names: ";
        cin >> inputFileName >> outputFileName;

        if(inputFileName.length() < 1)
        {
            cout << "^D" << endl;
            break;
        }
    
        ifstream inFile(inputFileName);
        ofstream outFile(outputFileName);
        if (!inFile) 
        {
            cout << "Unable to open " << inputFileName << endl;
            break;
        }
        if(!outFile)
        {
            cout << "Unable to open " << outputFileName << endl;
            break;
        }
        while(getline(inFile,inputTxt))
        {
            write(sockfd, inputTxt.c_str(), inputTxt.length()+1); 
            nread = read(sockfd, buffer, buflen);
            outFile << buffer << endl;
        }
        cout << "Conversion is complete." << endl;
   
        inFile.close();
        outFile.flush();  
        outFile.close();
    }
    
    close(sockfd);
    exit(0);
}






