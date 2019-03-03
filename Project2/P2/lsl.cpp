#define _SVID_SOURCE
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <iomanip>

using namespace std;

string trimEnd(string dir);
void lsl(string dir);
string numToMonth(int num);

int main(int argc, char** argv)
{
    string dir = ".";
    if(argc > 2)
    {
        cerr << "Usage: only 2 parameters are permitted" << endl;
        return 1;
    }
    if(argc > 1)
    {
        dir = argv[1];
    }
    
    string newDir = trimEnd(dir);
    lsl(newDir);
    
    return 0;
 }

string trimEnd(string dir)
{
    if(dir[dir.length()-1] == '/')
        dir = dir.substr(0, dir.length()-1);
    return dir;
}

void lsl (string dir)
{
    struct dirent **namelist;
    int n, i, j, fileSize, superSize;
    const char * path = dir.c_str();
    
    n = scandir(path, &namelist, NULL, alphasort);
    //cout << n << endl;
    if (n < 0)
        perror("scandir");
    else {
        //
        for (j = 2; j < n; j++)
        {
            struct stat SMeta;
            stat(namelist[j]->d_name, &SMeta);
            superSize += SMeta.st_blocks;
        }
        
        //Piece 1/8: Total size of files in directory
        cout << "total " << superSize << endl;
        for (i = 2; i < n; i++) //i=2 to ignore the '.' & '..' entries
        {
            mode_t sMode;
            struct stat SMeta;
            string const fullPath = dir + "/" + namelist[i]->d_name;
            stat(fullPath.c_str(), &SMeta);
            sMode = SMeta.st_mode;
            
            //Piece 2/8: 10-char file type + permissions data
            string permissions = "----------";
            //switch(sMode & S_IFMT)
            {
                if (sMode & S_IFDIR)
                    permissions[0] = 'd';
                if (S_ISLNK(sMode))//(sMode & S_IFLNK)
                    permissions[0] = 'l';
                if (sMode & S_IRUSR)
                    permissions[1] = 'r';
                if (sMode & S_IWUSR)
                    permissions[2] = 'w';
                if (sMode & S_IXUSR)
                    permissions[3] = 'x';
                if (sMode & S_IRGRP)
                    permissions[4] = 'r';
                if (sMode & S_IWGRP)
                    permissions[5] = 'w';
                if (sMode & S_IXGRP)
                    permissions[6] = 'x';
                if (sMode & S_IROTH)
                    permissions[7] = 'r';
                if (sMode & S_IWOTH)
                    permissions[8] = 'w';
                if (sMode & S_IXOTH)
                    permissions[9] = 'x';
            }
            //Piece 3/8: Number of files in directory
            //See the final 'cout' statement
            
            //Piece 4/8: The owner of the file
            struct passwd *pwd = getpwuid(SMeta.st_uid);
            
            //Piece 5/8: The group the file belongs to
            struct group *grp = getgrgid(SMeta.st_gid);
            
            //Piece 6/8: The size of the file
            fileSize = SMeta.st_size;
            
            //Piece 7/8: The date the file was modified last
            //time_t rawtime;
            time_t t = SMeta.st_mtime;
            //struct tm * timeinfo;
            //time (&rawtime);
            //timeinfo = localtime(&rawtime);
            char buffer[80];
            struct tm lt;
            localtime_r(&t, &lt);
            strftime(buffer,sizeof(buffer),"%m",&lt);
            string date2(buffer);
            strftime(buffer,sizeof(buffer),"%d %H:%M",&lt);
            string date1(buffer);
            string date = numToMonth(stoi(date2)) + " " + date1;
            
            
            //Piece 8/8: The name of the file (the last item in this statement)
            cout << permissions << " " << setw(2) << SMeta.st_nlink << " " << setw(7) << pwd->pw_name << " " << setw(6) << grp->gr_name << " " << setw(5) << fileSize <<" "<< date << " " << namelist[i]->d_name << endl;
            
            free(namelist[i]);
        }
        free(namelist);
        
    }
    
}

string numToMonth(int num)
{
    switch(num)
    {
        case 1:
            return "Jan";
            break;
        case 2:
            return "Feb";
            break;
        case 3:
            return "Mar";
            break;
        case 4:
            return "Apr";
            break;
        case 5:
            return "May";
            break;
        case 6:
            return "Jun";
            break;
        case 7:
            return "Jul";
            break;
        case 8:
            return "Aug";
            break;
        case 9:
            return "Sep";
            break;
        case 10:
            return "Oct";
            break;
        case 11:
            return "Nov";
            break;
        case 12:
            return "Dec";
            break;
    }
}
