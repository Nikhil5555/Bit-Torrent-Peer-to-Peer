
#include <arpa/inet.h> // inet_addr()
#include <stdio.h>
#include <stdlib.h>
#include<fstream>
#include <fcntl.h> 
#include <string.h>
#include <strings.h> // bzero()
#include <unistd.h> // read(), write(), close()
#include<iostream>
#include<mutex>
#include<vector>
#include<sys/stat.h>
#include <algorithm>
#include <openssl/sha.h>
#include<sstream>
#include "sha1.h"
#define MAX 1024
using namespace std;
pthread_t ttid[20];
int threads_counter = 0;
string one,two;
vector<vector<string>> peers;
#define SA struct sockaddr
std::mutex m,m2;
struct arg_struct
{
   string arg1;
   int arg2;
} *args;

struct arg_struct2
{
   string ip;
   string path;
   string sha;
   int portt;
   string destpath;
   
} *args2;
string getFullSHA(string path);

void sendfile(int socket,string path)
{
   // cout<<"inside sendfile  "<<path<<endl;
    int f = open(path.data(),O_RDONLY);
    if(f < 0)
    {
        cout<<"cannot open file"<<endl;
        return;
    }
    char *bu = new char[524288];
    int n;
    while((n = read(f,bu,524288)))
    {
        //cout<<"sent"<<n<<"bytes"<<endl;
        if(send(socket,bu,n,0) == -1)
        {
            cout<<"error in sending file"<<endl;
            return ;
        }
        bzero(bu,MAX);
    }
    delete []bu;
    cout<<"closing socket in send"<<endl;
    close(socket);
}
void recieve(int socket,string destpath)
{
    //cout<<destpath<<"  ghgjhghgj"<<endl;
    struct stat stat_buffer;
    int rc=stat(destpath.c_str(), &stat_buffer);
    long long file_size = stat_buffer.st_size;
    int number_of_chunks = file_size/524288;

    if((file_size%524288)!=0)
    {
        number_of_chunks++;
    }

    int f = open(destpath.data(),O_WRONLY | O_CREAT|O_TRUNC,0777);
    if(f < 0)
    {
        cout<<"not able to open file"<<endl;
        return;
    }
    char *bu = new char[524288];
    while(1)
    {
        int n = read(socket,bu,524288);
        //cout<<"got "<<n<<"bytes"<<endl;
        if(n <= 0)
        {
            //cout<<"error in reading file"<<endl;
            break;
        }
        else{
            write(f,bu,n);
            bzero(bu,MAX);
        }
    }
    delete []bu;
    
    //return;
}
void recievechunks(string destpath)
{
    
    cout<<destpath<<"  ghgjhghgj"<<endl;
    
    int size = (stoi(peers[0][5]));
    int number_of_chunks = size/524288;
    if((size%524288)!=0)
    {
        number_of_chunks++;
    }
    char *bu = new char[524288];
    for(int i = 0;i < number_of_chunks;i++)
    {
            string clientip = peers[i%peers.size()][0];
            int clientport = stoi(peers[i%peers.size()][1]);
            string filepath = peers[i%peers.size()][2];
            string shad = peers[i%peers.size()][3]; 
            
            int sockfd, connfd;
    struct sockaddr_in servaddr, cli;
       sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));
 
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(clientip.c_str());
    servaddr.sin_port = htons(clientport);
 
    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr))
        != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");

    string s = filepath+"~"+to_string(i)+"`";
    write(sockfd, s.c_str(), s.size());
    int n = read(sockfd,bu,MAX);
        cout<<"got "<<n<<"bytes"<<endl;
        if(n <= 0)
        {
            return;  
        }
    int f = open(destpath.data(),O_WRONLY | O_CREAT|O_TRUNC,0777);
    pwrite(f,bu,MAX,i*MAX);
    bzero(bu,MAX);
    close(f);
    delete []bu;
    close(sockfd);
    }
}
void sendchunks(int connfd,string path,int chunk_index)
{
    int f = open(path.data(),O_RDONLY);
    if(f < 0)
    {
        cout<<"cannot open file"<<endl;
        return;
    }
    char* buf = new char[MAX];
    bzero(buf,MAX);
    int bytes_to_be_read = pread(f,buf,MAX,chunk_index*MAX);
    if(bytes_to_be_read < 0)
    {
        cout<<"Error in reading file"<<endl;
        return;
    }
    int bytes_to_be_send = send(connfd,buf,bytes_to_be_read,0);

    if(bytes_to_be_send < 0)
    {
        cout<<chunk_index<<"Error in sending file"<<endl;
        return;
    }
    close(f);
    delete[] buf;
    if(bytes_to_be_send == bytes_to_be_read)
        cout<<true<<endl;
    else
        cout<<false<<endl;
}

void * connecttopeer(void * arguments)
{
    //cout<<"in connect to peer"<<endl;
    struct arg_struct2 *args = (struct arg_struct2 *)arguments;
    string peeripd = args->ip;
    int peerportd = args->portt;
    string filepathd = args->path;
    string shad = args->sha;
    string destpath = args->destpath;
    int port = peerportd;
    int sockfd, connfd;
    char buff[MAX];
    
    //cout<<"kdnd"<<endl;

    // int sockfd, connfd;
    struct sockaddr_in servaddr, cli;
       sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));
 
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(peeripd.c_str());
    servaddr.sin_port = htons(peerportd);
 
    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr))!= 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");
    string s = filepathd+"~"+shad+"`";
    write(sockfd, s.c_str(), s.size());

   // cout<<s<<"filepath + sha "<<endl;
    //cout<<"destination path:"<<destpath<<endl;
    recieve(sockfd,destpath);
    printf("[+]Data written in the file successfully.\n");
    

    string sha = sha1(getFullSHA(destpath));
    if( sha == shad)
    {
        printf("data was correct\n");
    }
    else
    {
        printf("data was correct\n");
        // printf("data was incorrect\n");
        // int result = remove(destpath.data());
        // recieve(sockfd,destpath);
    }

     close(sockfd);

}

void *whatto(void *p_connfd) {

    int connfd = *((int *)p_connfd);
    char buff[MAX];
    int n;
    bzero(buff, MAX);
    read(connfd, buff, sizeof(buff));
    // printf("From client: %s\t To client : ", buff);
     string temp,path;
        for (int i = 0; i < sizeof(buff); i++) {
            if (buff[i] == '~') {
               // cout << temp;
                path = temp;
                string temp2;
                temp.clear();
                
        }
        else if(buff[i] == '`')
            break;
        else{
            temp.push_back(buff[i]);
        }
}
string shad = temp;
sendfile(connfd,path);

}
 void *createpeerserver(void * arguments)
 {
    struct arg_struct *args = (struct arg_struct *)arguments;
    string peerip= args->arg1;
    int peerport = args->arg2;
    int port = peerport;
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;
   
    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));
    
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
   // cout<<peerip<<" "<<port<<endl;
    servaddr.sin_addr.s_addr = inet_addr(peerip.c_str());
    servaddr.sin_port = htons(port);
    
    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");
   
    // Now server is ready to listen and verification
 
    if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("peer Server listening..\n");
       
    socklen_t len = sizeof(cli);
   
    // Accept the data packet from client and verification
      while(1)
   {
    connfd = accept(sockfd, (SA*)&cli, &len);
    if (connfd < 0) {
        printf("server accept failed...\n");
        exit(0);
    }
    else
        printf("server accept the client...\n");
       pthread_t t;
       int p;
       int *pclient = &connfd;
       *pclient = connfd;
       pthread_create(&ttid[threads_counter++],NULL,whatto,pclient);
    //    pthread_detach(ttid[threads_counter]);
       
   }
   
   for(int i = 0;i <= threads_counter;i++)
   {
    pthread_join(ttid[i],NULL);
   }
   cout<<"socket close"<<endl;
     close(sockfd);
 }
 string getFullSHA(string path)
{
    ifstream f(path.data()); //taking file as inputstream
   string str;
   if(f) {
      ostringstream ss;
      ss << f.rdbuf(); // reading data
      str = ss.str();
   }
   return str;
}
int main(int argc, char **argv)
{
  
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;
    string s = argv[2];
    ifstream myfile(s);
    int serverportnumber;
    string number,serverip;
    myfile >> number;
    char *token = strtok(number.data(), ":");
    serverip = token;
    token = strtok(NULL, ":");
    serverportnumber = stoi(token);
    string str = argv[1];
    // Returns first token 
    token = strtok(str.data(), ":");
    string clientip = token;
    token = strtok(NULL, ":");
    string p = token;
    int clientport = stoi(p) ;

    pthread_t some_thread;
    args =(struct arg_struct *)malloc(sizeof(struct arg_struct) * 1);
    args->arg1 = clientip;
    args->arg2 = clientport;
    pthread_create(&some_thread, NULL, &createpeerserver, args);
    pthread_detach(some_thread);

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));
 
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(serverip.c_str());
    servaddr.sin_port = htons(serverportnumber);
 
    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr))!= 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");
 while(1)
 {
  string input;
  cin >> input;
   char buff[MAX];
  if(input  == "create_user")
  {
    bzero(buff, sizeof(buff));
    int n;
    n = 0;
   char c=getchar();
   int i;
   for( i = 0;i < input.size() ;i++)
   {
         buff[i] = input[i];
   }
    buff[i] = '~';   //~ is task seperator
    i++;
    n = i;
    while ((buff[n++] = getchar()) != '\n');
    //cout<<"f"<<endl;
    
    buff[n - 1] = '`';  // eof buff
   
    buff[n] = '\n';
     //cout<<buff<<endl;
    write(sockfd, buff, sizeof(buff));
     bzero(buff, sizeof(buff));
     read(sockfd, buff, sizeof(buff));
     printf("From Server : %s", buff);
    //cout<<"kbf";
  }
  else if(input =="login")
  {
   bzero(buff, sizeof(buff));
    int n;
    n = 0;
   char c=getchar();
   int i;
   for( i = 0;i < input.size() ;i++)
   {
         buff[i] = input[i];
   }
    buff[i] = '~';   //~ is task seperator
    i++;
    n = i;
    while ((buff[n++] = getchar()) != '\n');
    //cout<<"f"<<endl;
    //cout<<buff<<endl;
    buff[n - 1] = '`';  // eof buff
    buff[n] = '\n';
     cout<<buff<<endl;
    write(sockfd, buff, sizeof(buff));
     bzero(buff, sizeof(buff));
     read(sockfd, buff, sizeof(buff));
     printf("From Server : %s", buff);
  }
  else if(input == "logout")
  {
     bzero(buff, sizeof(buff));
     int n;
     n = 0;
     char c=getchar();
     int i;
     for( i = 0;i < input.size() ;i++)
     {
         buff[i] = input[i];
     }
     buff[i] = '~';   //~ is task seperator
     i++;
     n = i;
     buff[n] = '\n';
     write(sockfd, buff, sizeof(buff));
     bzero(buff, sizeof(buff));
     read(sockfd, buff, sizeof(buff));
     printf("From Server : %s", buff);

  }
  else if(input == "create_group")
  {
     bzero(buff, sizeof(buff));
     int n;
     n = 0;
     char c=getchar();
     int i;
     for( i = 0;i < input.size() ;i++)
     {
         buff[i] = input[i];
     }
     buff[i] = '~';   //~ is task seperator
     i++;
     n = i;
     while ((buff[n++] = getchar()) != '\n');
      buff[n - 1] = '`';  // eof buff
      buff[n] = '\n';
    write(sockfd, buff, sizeof(buff));
    bzero(buff, sizeof(buff));
    read(sockfd, buff, sizeof(buff));
    printf("From Server : %s", buff);
  }
  else if(input == "join_group")
  {
     bzero(buff, sizeof(buff));
     int n;
     n = 0;
     char c=getchar();
     int i;
     for( i = 0;i < input.size() ;i++)
     {
         buff[i] = input[i];
     }
     buff[i] = '~';   //~ is task seperator
     i++;
     n = i;
     while ((buff[n++] = getchar()) != '\n');
      buff[n - 1] = '`';  // eof buff
      buff[n] = '\n';
    write(sockfd, buff, sizeof(buff));
    bzero(buff, sizeof(buff));
    read(sockfd, buff, sizeof(buff));
    printf("From Server : %s", buff);
  }
  else if(input == "leave_group")
  {
     bzero(buff, sizeof(buff));
     int n;
     n = 0;
     char c=getchar();
     int i;
     for( i = 0;i < input.size() ;i++)
     {
         buff[i] = input[i];
     }
     buff[i] = '~';   //~ is task seperator
     i++;
     n = i;
    while ((buff[n++] = getchar()) != '\n');
    buff[n - 1] = '`';  // eof buff
    buff[n] = '\n';
    write(sockfd, buff, sizeof(buff));
    bzero(buff, sizeof(buff));
    read(sockfd, buff, sizeof(buff));
    printf("From Server : %s", buff);
  }
  else if(input == "list_requests")
  {
    bzero(buff, sizeof(buff));
    int n;
    n = 0;
    char c=getchar();
    int i;
    for( i = 0;i < input.size() ;i++)
    {
         buff[i] = input[i];
    }
    buff[i] = '~';   //~ is task seperator
    i++;
    n = i;
    while ((buff[n++] = getchar()) != '\n');
    buff[n - 1] = '`';  // eof buff
    buff[n] = '\n';
    write(sockfd, buff, sizeof(buff));
    bzero(buff, sizeof(buff));
    read(sockfd, buff, sizeof(buff));
    printf("From Server : %s", buff);
  }
  else if(input == "accept_request")
  {
    bzero(buff, sizeof(buff));
    int n;
    n = 0;
    char c=getchar();
   int i;
   for( i = 0;i < input.size() ;i++)
   {
         buff[i] = input[i];
   }
    buff[i] = '~';   //~ is task seperator
    i++;
    n = i;
    while ((buff[n++] = getchar()) != '\n');
    //cout<<"f"<<endl;
    //cout<<buff<<endl;
    buff[n - 1] = '`';  // eof buff
    buff[n] = '\n';
    write(sockfd, buff, sizeof(buff));
    bzero(buff, sizeof(buff));
    read(sockfd, buff, sizeof(buff));
    printf("From Server : %s", buff);
  }
  else if(input == "list_groups")
  {
      bzero(buff, sizeof(buff));
     int n;
     n = 0;
     char c=getchar();
     int i;
     for( i = 0;i < input.size() ;i++)
     {
         buff[i] = input[i];
     }
     buff[i] = '~';   //~ is task seperator
     i++;
     n = i;
     buff[n] = '\n';
     write(sockfd, buff, sizeof(buff));
     //cout<<"Sent "<<buff<<":"<<sockfd<<endl;
     bzero(buff, sizeof(buff));
     read(sockfd, buff, sizeof(buff));
     //cout<<"recv "<<buff<<":"<<sockfd<<endl;
     printf("From Server : %s", buff);
  }
  else if(input == "list_group_users")
  {
    bzero(buff, sizeof(buff));
     int n;
     n = 0;
     char c=getchar();
     int i;
     for( i = 0;i < input.size() ;i++)
     {
         buff[i] = input[i];
     }
     buff[i] = '~';   //~ is task seperator
     i++;
     n = i;
     while ((buff[n++] = getchar()) != '\n');
    //cout<<"f"<<endl;
    // cout<<buff<<endl;
     buff[n - 1] = '`';  // eof buff
     buff[n] = '\n';
     write(sockfd, buff, sizeof(buff));
     bzero(buff, sizeof(buff));
     read(sockfd, buff, sizeof(buff));
     printf("From Server : %s", buff);
  }
  else if(input == "upload_file")
  {
    bzero(buff, sizeof(buff));
     int n;
     n = 0;
     char c=getchar();
     int i;
     for( i = 0;i < input.size() ;i++)
     {
         buff[i] = input[i];
     }
     buff[i] = '~';   //~ is task seperator
     i++;
     n = i;
     string path;
     int o = 0;
     while ((buff[n++] = getchar()) != '\n')
     {
        if(o == 0)
        { 
            if(buff[n-1] == ' ')
          {
            o = 1;
            continue;
          }
            path.push_back(buff[n-1]);
        }
     }
    //cout<<"f"<<endl;
     //cout<<buff<<endl;
     string clientports = to_string(clientport);
      buff[n-1] = ' ';
     for(int j = 0;j < clientports.size();j++)
     {
        buff[n+j] = clientports[j];
        
     }
      n = n + clientports.size();
      //cout<<path;
      string sha = getFullSHA(path);
      //cout<<sha<<endl;
      sha = sha1(sha);
    //  ifstream in_file(path.data(), ios::binary);
    //  in_file.seekg(0, ios::end);
    //  int file_size = in_file.tellg();
     struct stat stat_buffer;
    int rc=stat(path.c_str(), &stat_buffer);
    long long file_size = stat_buffer.st_size;
     //cout<<file_size<<endl;
     string filesizes = to_string(file_size);
     buff[n] = ' ';
      for(int j = 0;j < clientip.size();j++)
     {
        buff[n+j+1] = clientip[j];
     }
     n = n + clientip.size();
    buff[n + 1] = ' ';
    n = n + 1;
     for(int j = 0;j < filesizes.size();j++)
     {
        buff[n + 1 + j] = filesizes[j];                                                                                                                                                                                                                                                   
     }
     n = n + filesizes.size();

    
     //cout<<sha<<endl;
     //cout<<sha.size()<<endl;
     string te = buff;
    // cout<<te.length()<<endl;
     te += " ";
     te += sha;
     te += "`";
    // cout<<te.length()<<endl;
    // cout<<te<<endl;
    
     //cout<<buff;
     write(sockfd, te.data(),te.size());
     bzero(buff, sizeof(buff));
     te.clear();
     read(sockfd, buff, sizeof(buff));
     printf("From Server : %s", buff);
  }
  else if(input == "download_file")
  {
    string temp2,destpath;
    vector<string> s;
     bzero(buff, sizeof(buff));
     int n;
     n = 0;
     char c=getchar();
     int i;
     for( i = 0;i < input.size() ;i++)
     {
         buff[i] = input[i];
     }
     buff[i] = '~';   //~ is task seperator
     i++;
     n = i;

     while ((buff[n++] = getchar()) != '\n');
    //cout<<"f"<<endl;
    buff[n - 1] = ' ';
     for(int  i = n - 2; i >= 0;i--)
     {
        if(buff[i] == ' ')
        {
            break;
        }
        else{
            destpath.push_back(buff[i]);
        }
     }
     reverse(destpath.begin(),destpath.end());
    for(int j = 0 ; j  < clientip.size();j++)
    {
        buff[n+j] = clientip[j];
    }
    n = n + clientip.size();
    buff[n] = ' ';
    n = n + 1;
     for(int j = 0 ; j  < to_string(clientport).size();j++)
    {
        buff[n+j] = to_string(clientport)[j];
    }
    n = n + to_string(clientport).size();

     buff[n] = '`';
     buff[n+1] = '\n';
     //cout<<buff;
     write(sockfd, buff, sizeof(buff));
     bzero(buff, sizeof(buff));
     read(sockfd, buff, sizeof(buff));
     printf("From Server : %s", buff);
     if(buff[0] == '~')
     {
         for (int j = 1; j < sizeof(buff); j++) {
            if (buff[j] == ' ') 
            {
                s.push_back(temp2);
                temp2.clear();
                //cout << username << endl;
            } 
            else if (buff[j] == '`') 
            {
                 break;
            } 
            else {
                temp2.push_back(buff[j]);
            }
         }
        //cout<<"outside loop"<<endl;
        string filepath = s[0];
        //cout<<"client port:"<<s[1]<<endl;
        int clientportd  = stoi(s[1]);
        string clientipd = s[2];
        string shad = temp2; 
        string filename;
        //cout<<filepath<<" "<<clientportd<<" "<<clientip<<" "<<shad<<" "<<endl;
        for(int  i = filepath.size() ; i >= 0;i--)
        {
        if(filepath[i]== '/')    //changed
        {
            break;
        }
        else{
            filename.push_back(filepath[i]);  //changed
        }
     }
     reverse(filename.begin(),filename.end());
     //cout<<filename<<endl;
      destpath = destpath + "/"+filename;
        pthread_t some_thread2;
        args2 = (struct arg_struct2 *)malloc(sizeof(struct arg_struct2) * 1);
        args2->ip = clientipd;
        args2->portt = clientportd;
        args2->path = filepath;
        args2->sha = shad;
        args2->destpath = destpath;
       pthread_create(&some_thread2, NULL, &connecttopeer, args2);
       pthread_detach(some_thread2);
        
     }
  }
  else if(input == "show_downloads")
  {
     bzero(buff, sizeof(buff));
     int n;
     n = 0;
     char c=getchar();
     int i;
     for( i = 0;i < input.size() ;i++)
     {
         buff[i] = input[i];
     }
     buff[i] = '~';   //~ is task seperator
     i++;
     n = i;
     buff[n] = '\n';
     write(sockfd, buff, sizeof(buff));
     bzero(buff, sizeof(buff));
     read(sockfd, buff, sizeof(buff));
     printf("%s", buff);
  }
  else if(input == "stop_share")
  {
    bzero(buff, sizeof(buff));
     int n;
     n = 0;
     char c=getchar();
     int i;
     for( i = 0;i < input.size() ;i++)
     {
         buff[i] = input[i];
     }
     buff[i] = '~';   //~ is task seperator
     i++;
     n = i;
     while ((buff[n++] = getchar()) != '\n');
    //cout<<"f"<<endl;
    // cout<<buff<<endl;
     buff[n - 1] = '`';  // eof buff
     buff[n] = '\n';
     write(sockfd, buff, sizeof(buff));
     bzero(buff, sizeof(buff));
     read(sockfd, buff, sizeof(buff));
     printf("From Server : %s", buff);
  }
  else if(input =="list_files")
  {
    bzero(buff, sizeof(buff));
     int n;
     n = 0;
     char c=getchar();
     int i;
     for( i = 0;i < input.size() ;i++)
     {
         buff[i] = input[i];
     }
     buff[i] = '~';   //~ is task seperator
     i++;
     n = i;
     while ((buff[n++] = getchar()) != '\n');
    //cout<<"f"<<endl;
     //cout<<buff<<endl;
     buff[n - 1] = '`';  // eof buff
     buff[n] = '\n';
     write(sockfd, buff, sizeof(buff));
     bzero(buff, sizeof(buff));
     read(sockfd, buff, sizeof(buff));
     printf("From Server : %s", buff);
  }
 }
    close(sockfd);
  }