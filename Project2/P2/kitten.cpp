#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <getopt.h>

using namespace std;

void displayText(istream &file, bool options[]);

int main(int argc, char **argv)
{
    static const char * optString = "Ens";
    int opt;
    bool options [3] = {false, false, false};
    ifstream in;
    
    opt = getopt(argc, argv, optString);
    while(opt != -1 )
    {
        switch(opt)
        {
            case 'E':
                options[0] = true;
                break;
            case 'n':
                options[1] = true;
                break;
            case 's':
                options[2] = true;
                break;
            case '?':
                //cout << "You are using an altered form of cat that supports, -E, -n, and -s"
                break;
            default:
                break;
        }
        opt = getopt( argc, argv, optString );
    }
    
    if (optind >= argc) //no file provided
    {
        displayText(cin, options);
    }
    
    else
    {
        while(optind < argc) //file(s) provided
        {
            in.open(argv[optind]);
            displayText(in, options);
            in.close();
            optind++;
        }
    }
    return 0;
}

void displayText(istream &in, bool options[])
{
    string line;
    int lines = 1;
    
    while(getline(in, line))
    {
        if(options[2] && line == "")
        {
            continue;
        }
        if(options[1]) //number lines
        {
            cout << lines << " ";
            lines++;
        }
        cout << line; //original line
        
        if(options[0]) //append $ to line
        {
            cout << "$";
        }
        cout << endl;
    }
}
