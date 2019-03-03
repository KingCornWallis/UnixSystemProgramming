#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <netdb.h>
#include <vector>

using namespace std;

int playerFD[2] = {0, 0};
int inputChoice[2] = {-1, -1};
string inputMessage[2] = {"", ""};
string ouputMessage[2] = {"", ""};
int score[2] = {0, 0};



int maxClients = 2;

void e_error(const char *m);
void ee_error(const char *m, int errcode);
void scoreHandler(int winner);
int gameHandler(int choice1, int choice2);
void sendMessage(string generalOutput);
void shutDown(fd_set &readfds);


int main()
{
  string generalOutput;
  string generalInput;

  int s_sfd, c_sfd;
  struct sockaddr s_addr, c_addr;
  socklen_t s_len, c_len;
  struct addrinfo hints, *res;
  
  fd_set readfds, testfds;
  static char buffer[128];
  int buflen = sizeof(buffer);
  int result;
  string input;
  int flags;
  char name[1024];
  char port[1024];

  //vector<int> choices;


  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  result = getaddrinfo(NULL, "0", &hints, &res);

  if(result == -1)
    e_error("Failed to bind");

  s_sfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if(s_sfd == -1)
    e_error("socket");
  

  result = bind(s_sfd, res->ai_addr, res->ai_addrlen);    
  if(result == -1)
    e_error("bind");

  s_len = sizeof(s_addr);
  result = getsockname(s_sfd, &s_addr, &s_len);
  if(result == -1)
    e_error("Could not get socket name");

    
  flags = NI_NUMERICHOST | NI_NUMERICSERV;
  result = getnameinfo(&s_addr, sizeof(s_addr),
		name, sizeof(name), port, sizeof(port), flags);
  if(result != 0)
    ee_error("getnameinfo", result);
  //cout << "IP: " << name << "\t\tPORT: " << port << endl;
  cout << "referee is using port " << port << endl;
  freeaddrinfo(res);

  listen(s_sfd, 5);

  FD_ZERO(&readfds);
  FD_SET(s_sfd, &readfds);

  
  int playercount = 0;
  while(1) 
  {
    int fd;
    int nread;

    testfds = readfds;
    //maxfdp1 = maxClients;
    cout << "Referee is waiting for players" << endl;
    result = select(maxClients, &testfds, (fd_set *)0, (fd_set *)0, (struct timeval *) 0);
    if(result < 1) 
    {
      cerr << "server: error in select call" << endl;
      exit(1);
    }

    for(fd = 0; fd < maxClients; fd++) 
    {
      if(FD_ISSET(fd,&testfds)) 
      {
        if(fd == s_sfd) 
        {
          c_len = sizeof(c_addr);
          if (playercount == 0)
          {
            playerFD[0] = accept(s_sfd, &c_addr, &c_len);
            FD_SET(playerFD[0], &readfds);
            cout << "Player 1 has connected (and is using fd " << playerFD[0] << ")" << endl;

            maxClients++;
            playercount += 1;
          }
          else if(playercount == 1)
          {
            playerFD[1] = accept(s_sfd, &c_addr, &c_len);
            FD_SET(playerFD[1], &readfds);
            cout << "Player 2 has connected (and is using fd " << playerFD[1] << ")" << endl;
            maxClients++;
            playercount += 1;
      
            generalOutput = "GO";
		        sendMessage(generalOutput);

		        generalOutput = "1";
            write(playerFD[0], generalOutput.c_str(), generalOutput.length()+1);
		        generalOutput = "2";
            write(playerFD[1], generalOutput.c_str(), generalOutput.length()+1);
          }
        }

        else 
        {
		     
		      generalOutput.clear();
		      while(1) 
          {
            nread = read(fd, buffer, buflen);
            if(nread == 0 || strcmp(buffer, "STOP") == 0)
            {
              generalOutput = "STOP";
              sendMessage(generalOutput);


              close(playerFD[0]);
              close(playerFD[1]);
              FD_CLR(playerFD[0], &readfds);
              FD_CLR(playerFD[1], &readfds);
              cout << "Game has ended"<< endl;

              playercount = 0;
              maxClients -= 2;
              break;
            }
            if(buffer[nread-1] == '\0') 
            {
		          generalOutput.append(buffer, nread-1);
		          break;
		        }
            else
	            generalOutput.append(buffer, nread);
		      }

          if(playercount == 2)
          {
		        while(1)
            {
		          nread = read(playerFD[0], buffer, buflen);
		          inputMessage[0] = buffer;
		          nread = read(playerFD[1], buffer, buflen);
		          inputMessage[1] = buffer;
		 
		          if((inputMessage[0] == "0") || (inputMessage[1] == "0"))
              {
			          generalOutput = "STOP";
			          cout << "writing stop " << endl;
                sendMessage(generalOutput);

                generalOutput = score[0];
                sendMessage(generalOutput);


                generalOutput = score[1]; 
                sendMessage(generalOutput);

                shutDown(readfds);
                close(playerFD[0]);
                close(playerFD[1]);
                FD_CLR(playerFD[0], &readfds);
                FD_CLR(playerFD[1], &readfds);
                playercount = 0;
                maxClients -= 2;
                break;
		          }
              inputChoice[0] = atoi(inputMessage[0].c_str());
              inputChoice[1] = atoi(inputMessage[1].c_str());

		          int player = gameHandler(inputChoice[0], inputChoice[1]);
		
		          scoreHandler(player);	
	
		          if (player == 0)
		            generalOutput = "Tie!";
		          else if (player == 1)
		            generalOutput = "Player 1 won!";
              else if (player == 2)
                generalOutput = "Player 2 won!";
              sendMessage(generalOutput);
  
		        }
          }
        }
      }
    }
  }
  return 0;
}

void e_error(const char *m)
{
  cerr << m << ": " << strerror(errno) << endl;
  exit(errno);
}

void ee_error(const char *m, int errcode)
{
  cerr << m << ": " << gai_strerror(errcode) << endl;
  exit(errcode);
}

void scoreHandler(int winner)
{
  if(winner == 1)
    score[0] += 1;
  else if(winner == 2)
    score[1] += 1;
}

int gameHandler(int choice1, int choice2)
{
  if(choice1 == choice2)
    return 0;
  if(choice1 == 1 && choice2 == 2)
    return 2;
  if(choice1 == 1 && choice2 == 3)
    return 1;
  if(choice1 == 2 && choice2 == 1)
    return 1;
  if(choice1 == 2 && choice2 == 3)
    return 2;
  if(choice1 == 3 && choice2 == 1)
    return 2;
  if(choice1 == 3 && choice2 == 2)
    return 1;
}

void sendMessage(string generalOutput)
{
  for (int i = 0; i < sizeof(playerFD); i++)
  {
    write(playerFD[i], generalOutput.c_str(), generalOutput.length()+1);
  }
}

void shutDown(fd_set &readfdfs)
{
  for (int i = 0; i < sizeof(playerFD); i++)
  {

  }
}