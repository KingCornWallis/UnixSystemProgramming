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

using namespace std;
void e_error(const char *m);
void ee_error(const char *m, int errcode);
void to_upper_string(char * str);

int main()
{
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
  cout << "server accepting clients on fd " << s_sfd << endl;

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
  cout << "IP: " << name << "\t\tPORT: " << port << endl;
  freeaddrinfo(res);

  listen(s_sfd, 5);

  FD_ZERO(&readfds);
  FD_SET(s_sfd, &readfds);

  int nmax = s_sfd+1, maxfdp1;
  while(1) 
  {
    int fd;
    int nread;

    testfds = readfds;
    maxfdp1 = nmax;
    cout << "server waiting" << endl;
    result = select(maxfdp1, &testfds, (fd_set *)0, (fd_set *)0, (struct timeval *) 0);
    if(result < 1) 
    {
      cerr << "server: error in select call" << endl;
      exit(1);
    }

    for(fd = 0; fd < maxfdp1; fd++) 
    {
      if(FD_ISSET(fd,&testfds)) 
      {
        if(fd == s_sfd) 
        {
          c_len = sizeof(c_addr);
          c_sfd = accept(s_sfd, &c_addr, &c_len);
          FD_SET(c_sfd, &readfds);
          cout << "adding client on fd " << c_sfd << endl;
          nmax = max(maxfdp1, c_sfd+1);
        }

        else 
        {
		     
		      input.clear();
		      while(true) 
          {
            nread = read(fd, buffer, buflen);
            if(nread == 0) 
            {
              close(fd);
              FD_CLR(fd, &readfds);
              cout << "removing client on fd " << fd << endl;
              break;
            }
            if(buffer[nread-1] == '\0') 
            {
		          input.append(buffer, nread-1);
		          break;
		        }
            else
	            input.append(buffer, nread);
		      }
          char *modInput = new char[input.length() + 1];
          strcpy(modInput, input.c_str());
          to_upper_string(modInput);
          write(fd, modInput, input.length() + 1);
         
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

void to_upper_string(char * str)
{
    locale loc;
    for (size_t i=0; i <strlen(str); ++i)
        str[i] = toupper(str[i],loc);
    str[255] = '\0';
}