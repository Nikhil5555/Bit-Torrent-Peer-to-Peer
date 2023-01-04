#include <arpa/inet.h> // inet_addr()
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <openssl/sha.h>
#include <algorithm>
#define MAX 1024
#define SA struct sockaddr
#include <pthread.h>
#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <unordered_map>
#include <vector>
using namespace std;
struct arg_struct
{
   struct sockaddr_in arg1;
   int arg2;
}*args;

pthread_t ttid[20];
int threads_counter = 0;
string one, two;
unordered_map<string, string> users;
unordered_map<string,vector<pair<string,string>>> showdw;
unordered_map<string, bool> loggedin;
unordered_map<string, vector<string>> group;
unordered_map<string, vector<vector<string>>> groupfiles;
unordered_map<string, vector<string>> grouppendinglists;

std::mutex m, m2;
void logout(string username, int connfd) {
    loggedin[username] = false;
    //cout << "logged out successfully" << endl;
    char buff[MAX];
    bzero(buff, MAX);
    string c = "logged out successfully";
    int i;
    for (i = 0; i < c.size(); i++) {
        buff[i] = c[i];
    }
    buff[i] = '\n';
    write(connfd, buff, sizeof(buff));
}
bool login(string username, string password, int connfd) {
    //cout << "inside login" << endl;
    m2.lock();
    if (users[username] == password) {
       // cout << "login successful" << endl;
        char buff[MAX];
        bzero(buff, MAX);
        string c = "login successfull";
        int i;
        for (i = 0; i < c.size(); i++) {
            buff[i] = c[i];
        }
        buff[i] = '\n';
        write(connfd, buff, sizeof(buff));
        loggedin[username] = true;
       // cout << "logggggggg"<<endl;
        m2.unlock();
        return true;
    } else {
        char buff[MAX];
        bzero(buff, MAX);
        string c = "login failed";
        int i;
        for (i = 0; i < c.size(); i++) {
            buff[i] = c[i];
        }
        buff[i] = '\n';
        write(connfd, buff, sizeof(buff));
        m2.unlock();
        return false;
    }
}
void create_userr(string username, string password, int connfd) {
    m.lock();
    users[username] = password;
    // ofstream stream;  // To Write into a File, Use "ofstream"
    // stream.open("create_user.txt", std::ios_base::app);
    char buff[MAX];
    bzero(buff, MAX);
    string c = "user registered";
    int i;
    for (i = 0; i < c.size(); i++) {
        buff[i] = c[i];
    }
    buff[i] = '\n';
    write(connfd, buff, sizeof(buff));
    // stream << username << " " << password << '\n';
    // stream.close();
    m.unlock();
}
void create_group(string groupid, string username, int connfd) {
    if (group.find(groupid) == group.end()) {
        group[groupid].push_back(username);
        char buff[MAX];
        bzero(buff, MAX);
        string c = "group successfully created";
        int i;
        for (i = 0; i < c.size(); i++) {
            buff[i] = c[i];
        }
        buff[i] = '\n';
        write(connfd, buff, sizeof(buff));
    } else {
        char buff[MAX];
        bzero(buff, MAX);
        string c = "group already exists";
        int i;
        for (i = 0; i < c.size(); i++) {
            buff[i] = c[i];
        }
        buff[i] = '\n';
        write(connfd, buff, sizeof(buff));
    }
}
void leave_group(string groupid, string username, int connfd) {
    if (group.find(groupid) != group.end()) {
        if (find(group[groupid].begin(), group[groupid].end(), username) !=
            group[groupid].end()) {
            group[groupid].erase(find(group[groupid].begin(), group[groupid].end(), username));
            //vector<vector<string>> d = groupfiles[groupid];
            // vector<vector<string>>::iterator it;
            // it = groupfiles[groupid].begin();
            for(int i = 0; i < groupfiles[groupid].size();i++)
            {
                if(groupfiles[groupid][i][5] == username)
                {
                    groupfiles[groupid].erase(groupfiles[groupid].begin() + i);
                    i--;
                }
                // else{
                //     it++;
                // }
            }
            // loggedin.erase(username);
            char buff[MAX];
            bzero(buff, MAX);
            string c = "group leaved successfully";
            int i;
            for (i = 0; i < c.size(); i++) {
                buff[i] = c[i];
            }
            buff[i] = '\n';
            write(connfd, buff, sizeof(buff));
        } else {
            char buff[MAX];
            bzero(buff, MAX);
            string c = "you are not in the group";
            int i;
            for (i = 0; i < c.size(); i++) {
                buff[i] = c[i];
            }
            buff[i] = '\n';
            write(connfd, buff, sizeof(buff));
        }
    } else {
        char buff[MAX];
        bzero(buff, MAX);
        string c = "group does not exist";
        int i;
        for (i = 0; i < c.size(); i++) {
            buff[i] = c[i];
        }
        buff[i] = '\n';
        write(connfd, buff, sizeof(buff));
    }
}
void join_group(string groupid, string username, int connfd) {
    if (group.find(groupid) != group.end()) {
        if (find(group[groupid].begin(), group[groupid].end(), username) ==
            group[groupid].end()) {
            if (find(grouppendinglists[groupid].begin(),
                     grouppendinglists[groupid].end(),
                     username) == grouppendinglists[groupid].end()) {
                grouppendinglists[groupid].push_back(username);
                // for (auto i : grouppendinglists) {
                //     cout << i.first << endl;
                //     for (auto j : i.second) {
                //         cout << j << " ";
                //     }
                //     cout << endl;
                // }

                char buff[MAX];
                bzero(buff, MAX);
                string c = "group joinning request sent";
                int i;
                for (i = 0; i < c.size(); i++) {
                    buff[i] = c[i];
                }
                buff[i] = '\n';
                write(connfd, buff, sizeof(buff));
            } else {
                char buff[MAX];
                bzero(buff, MAX);
                string c = "group joinning request already sent";
                int i;
                for (i = 0; i < c.size(); i++) {
                    buff[i] = c[i];
                }
                buff[i] = '\n';
                write(connfd, buff, sizeof(buff));
            }
        } else {
            char buff[MAX];
            bzero(buff, MAX);
            string c = "you are already in the group";
            int i;  // for (auto i : grouppendinglists) {
                //     cout << i.first << endl;
                //     for (auto j : i.second) {
                //         cout << j << " ";
                //     }
                //     cout << endl;
                // }
            for (i = 0; i < c.size(); i++) {
                buff[i] = c[i];
            }
            buff[i] = '\n';
            write(connfd, buff, sizeof(buff));
        }
    } else {
        char buff[MAX];
        bzero(buff, MAX);
        string c = "group does not exist";
        int i;
        for (i = 0; i < c.size(); i++) {
            buff[i] = c[i];
        }
        buff[i] = '\n';
        write(connfd, buff, sizeof(buff));
    }
}
void list_requests(string groupid, string username, int connfd) {
    if (group.find(groupid) != group.end()) {
        string admin = group[groupid][0];
        if (admin == username) {
            // groupp[groupid].push_back(username);
            char buff[MAX];
            bzero(buff, MAX);
            string c;
            for (auto k : grouppendinglists[groupid]) {
                c += k;
                c += " ";
            }
            // for(auto k : group[groupid])
            // {
            //     c+=k;
            //     c+=" ";
            // }

            int i;
            for (i = 0; i < c.size(); i++) {
                buff[i] = c[i];
            }
            buff[i] = '\n';
            write(connfd, buff, sizeof(buff));
        } else {
            char buff[MAX];
            bzero(buff, MAX);
            string c = "you are not admin";
            int i;
            for (i = 0; i < c.size(); i++) {
                buff[i] = c[i];
            }
            buff[i] = '\n';
            write(connfd, buff, sizeof(buff));
        }
    } else {
        char buff[MAX];
        bzero(buff, MAX);
        string c = "group doesnot exist";
        int i;
        for (i = 0; i < c.size(); i++) {
            buff[i] = c[i];
        }
        buff[i] = '\n';
        write(connfd, buff, sizeof(buff));
    }
}
void list_groups(int connfd) {
    char buff[MAX];
    bzero(buff, MAX);
    string c;
    for (auto k : group) {
        c += k.first;
        c += " ";
    }
    int i;
    for (i = 0; i < c.size(); i++) {
        buff[i] = c[i];
    }
    buff[i] = '\n';
    write(connfd, buff, sizeof(buff));
}
void accept_request(string groupid, string username, string user, int connfd) {
    if (group.find(groupid) != group.end()) {
        string admin = group[groupid][0];
        if (admin == username) {
            // groupp[groupid].push_back(username);
            if (find(grouppendinglists[groupid].begin(),grouppendinglists[groupid].end(), user) != grouppendinglists[groupid].end()) {
                group[groupid].push_back(user);
                grouppendinglists[groupid].erase(find(grouppendinglists[groupid].begin(),grouppendinglists[groupid].end(), user));
                char buff[MAX];
                bzero(buff, MAX);
                string c = user + " was added in the group";
                int i;
                for (i = 0; i < c.size(); i++) {
                    buff[i] = c[i];
                }
                buff[i] = '\n';
                write(connfd, buff, sizeof(buff));
            } else {
                char buff[MAX];
                bzero(buff, MAX);
                string c = "request from this user does not exist";
                int i;
                for (i = 0; i < c.size(); i++) {
                    buff[i] = c[i];
                }
                buff[i] = '\n';
                write(connfd, buff, sizeof(buff));
            }
        } else {
            char buff[MAX];
            bzero(buff, MAX);
            string c = "you are not admin";
            int i;
            for (i = 0; i < c.size(); i++) {
                buff[i] = c[i];
            }
            buff[i] = '\n';
            write(connfd, buff, sizeof(buff));
        }
    } else {
        char buff[MAX];
        bzero(buff, MAX);
        string c = "group doesnot exist";
        int i;
        for (i = 0; i < c.size(); i++) {
            buff[i] = c[i];
        }
        buff[i] = '\n';
        write(connfd, buff, sizeof(buff));
    }
}
void list_group_users(string groupid, int connfd) {
    if (group.find(groupid) != group.end()) {
        char buff[MAX];
        bzero(buff, MAX);
        string c;
        for (auto k : group[groupid]) {
            c += k;
            c += " ";
        }

        int i;
        for (i = 0; i < c.size(); i++) {
            buff[i] = c[i];
        }
        buff[i] = '\n';
        write(connfd, buff, sizeof(buff));
    } else {
        char buff[MAX];
        bzero(buff, MAX);
        string c = "group does not exist";
        int i;
        for (i = 0; i < c.size(); i++) {
            buff[i] = c[i];
        }
        buff[i] = '\n';
        write(connfd, buff, sizeof(buff));
    }
}
void download_file(string groupid, string current_user,string destination_path,string downloadfilename,int connfd,string clientipd,string clientportd)
{
     if (group.find(groupid) != group.end()) {
        vector<vector<string>> d = groupfiles[groupid];
        int flag = -1;
        int i;
        for( i = 0;i < d.size();i++)
        {
           
                if(d[i][0] == downloadfilename)
                {
                    if(loggedin[d[i][5]] == false)
                    {
                        flag = 1;
                        continue;
                    }
                    else{
                        flag = 0;
                        break;
                    }
                }
        }           if(flag == 0)
                    {
                    char buff[MAX];
                    bzero(buff, MAX);
                    // cout<<"in download"<<endl;
                   // cout<<destination_path<<         "inside download_file"<<endl;
                    string c = "~"+d[i][1]+" "+d[i][3]+" "+d[i][4]+" "+d[i][6]+"`";  //filesize remaining
                    vector<string> tempstrings;
                    string p = (destination_path+"/"+downloadfilename);
                    //cout<<p<<endl;
                    tempstrings.push_back(downloadfilename);
                    // tempstrings.push_back(destination_path+'/'+downloadfilename);        //filepath
                    tempstrings.push_back(p);
                    tempstrings.push_back(d[i][2]); //filesize
                    tempstrings.push_back(clientportd);
                    tempstrings.push_back(clientipd);
                    tempstrings.push_back(current_user);
                    tempstrings.push_back(d[i][6]);
                    groupfiles[groupid].push_back(tempstrings);
                    //pair<string,string> p;
                    // p.first = groupid;
                    // p.second = downloadfilename;      
                    showdw[current_user].push_back({groupid,downloadfilename}) ;
                    

                    for (i = 0; i < c.size(); i++) {
                        buff[i] = c[i];
                    }
                    buff[i] = '\n';
                   // cout<<buff;
                    write(connfd, buff, sizeof(buff));
                    return;
                    }
                    else if(flag == 1){
                        char buff[MAX];
                        bzero(buff, MAX);
                         string c = "File exist,but no one is online";
                        int i;
                        for (i = 0; i < c.size(); i++) {
                            buff[i] = c[i];
                        }
                        buff[i] = '\n';
                        write(connfd, buff, sizeof(buff));
                    }
                    else
                    {
                        char buff[MAX];
                        bzero(buff, MAX);
                        string c = "file doesnt exist";
                        int i;
                        for (i = 0; i < c.size(); i++) {
                            buff[i] = c[i];
                        }
                        buff[i] = '\n';
                        write(connfd, buff, sizeof(buff));
                    }
                
        }
        
     else {
        char buff[MAX];
        bzero(buff, MAX);
        string c = "group doesnt exist";
        int i;
        for (i = 0; i < c.size(); i++) {
            buff[i] = c[i];
        }
        buff[i] = '\n';
        write(connfd, buff, sizeof(buff));
    }

}
void *whatto(void *p_connfd) {
    int connfd = *((int *)p_connfd);
    string current_user = " ";
    for (;;) {
        char buff[MAX];
        int n;
        bzero(buff, MAX);
        int e = read(connfd, buff, sizeof(buff));
        //cout<<"buff----------------------------"<<buff<<endl;
        if(e <= 0)
        {
            return NULL;
        }
        //read(connfd, buff, sizeof(buff));
        string task, temp, username, password, groupid, file_size, clientport,filepath,clientip,sha,destination_path,downloadfilename;
        vector<string> s;

        // printf("From client: %s\t To client : ", buff);

        for (int i = 0; i < sizeof(buff); i++) {
            if (buff[i] == '~') {
                //cout << temp;
                task = temp;
                string temp2;
                if (task == "create_user") {
                    for (int j = i + 1; j < sizeof(buff); j++) {
                        if (buff[j] == ' ') {
                            username = temp2;
                            temp2.clear();
                           // cout << username << endl;
                        } else if (buff[j] == '`') {
                            break;
                        } else {
                            temp2.push_back(buff[j]);
                        }
                    }
                    password = temp2;
                    create_userr(username, password, connfd);
                    break;
                } else if (task == "login") {
                    //cout << "gkrgkgrk";
                    for (int j = i + 1; j < sizeof(buff); j++) {
                        if (buff[j] == ' ') {
                            username = temp2;
                            temp2.clear();
                            //cout << username << endl;
                        } else if (buff[j] == '`') {
                            break;
                        } else {
                            temp2.push_back(buff[j]);
                        }
                    }
                    password = temp2;
                    //cout << password << endl;
                    // cout<<login(username,password);
                    if (login(username, password, connfd) == true) {
                        //cout << "username" << username;
                        current_user = username;
                        break;

                    } else {
                        //cout << "false";
                        break;
                    }
                    // break;
                } else if (task == "logout") {
                    logout(current_user, connfd);
                    break;
                } else if (task == "create_group") {
                   // cout << "vhjuthju";
                   // cout << current_user << endl;
                    if (loggedin.find(current_user) != loggedin.end() &&
                        loggedin[current_user] == true) {
                        for (int j = i + 1; j < sizeof(buff); j++) {
                            if (buff[j] == '`') {
                                groupid = temp2;
                                temp2.clear();
                                // cout << groupid << endl;
                                break;
                            } else {
                                temp2.push_back(buff[j]);
                            }
                        }
                        create_group(groupid, current_user, connfd);
                        break;
                    } else {
                        bzero(buff, MAX);
                        string c = "please login before doing this";
                        int i;
                        for (i = 0; i < c.size(); i++) {
                            buff[i] = c[i];
                        }
                        buff[i] = '\n';
                        write(connfd, buff, sizeof(buff));
                        break;
                    }

                } else if (task == "join_group") {
                   // cout << current_user << endl;
                    if (loggedin.find(current_user) != loggedin.end() &&
                        loggedin[current_user] == true) {
                        for (int j = i + 1; j < sizeof(buff); j++) {
                            if (buff[j] == '`') {
                                groupid = temp2;
                                temp2.clear();
                                // cout << groupid << endl;
                                break;
                            } else {
                                temp2.push_back(buff[j]);
                            }
                        }
                        join_group(groupid, current_user, connfd);
                        break;
                    } else {
                        bzero(buff, MAX);
                        string c = "please login before doing this";
                        int i;
                        for (i = 0; i < c.size(); i++) {
                            buff[i] = c[i];
                        }
                        buff[i] = '\n';
                        write(connfd, buff, sizeof(buff));
                        break;
                    }

                } else if (task == "leave_group") {
                   // cout << current_user << endl;
                    if (loggedin.find(current_user) != loggedin.end() &&
                        loggedin[current_user] == true) {
                        for (int j = i + 1; j < sizeof(buff); j++) {
                            if (buff[j] == '`') {
                                groupid = temp2;
                                temp2.clear();
                                // cout << groupid << endl;
                                break;
                            } else {
                                temp2.push_back(buff[j]);
                            }
                        }
                        leave_group(groupid, current_user, connfd);
                        break;
                    } else {
                        bzero(buff, MAX);
                        string c = "please login before doing this";
                        int i;
                        for (i = 0; i < c.size(); i++) {
                            buff[i] = c[i];
                        }
                        buff[i] = '\n';
                        write(connfd, buff, sizeof(buff));
                        break;
                    }

                } else if (task == "list_requests") {
                    if (loggedin.find(current_user) != loggedin.end() &&
                        loggedin[current_user] == true) {
                        for (int j = i + 1; j < sizeof(buff); j++) {
                            if (buff[j] == '`') {
                                groupid = temp2;
                                temp2.clear();
                              //  cout << groupid << endl;
                                break;
                            } else {
                                temp2.push_back(buff[j]);
                            }
                        }
                        list_requests(groupid, current_user, connfd);
                        break;
                    } else {
                        bzero(buff, MAX);
                        string c = "please login before doing this";
                        int i;
                        for (i = 0; i < c.size(); i++) {
                            buff[i] = c[i];
                        }
                        buff[i] = '\n';
                        write(connfd, buff, sizeof(buff));
                        break;
                    }
                } else if (task == "accept_request") {
                 //   cout << current_user << endl;
                    if (loggedin.find(current_user) != loggedin.end() &&
                        loggedin[current_user] == true) {
                        for (int j = i + 1; j < sizeof(buff); j++) {
                            if (buff[j] == ' ') {
                                groupid = temp2;
                                temp2.clear();
                             //   cout << username << endl;
                            } else if (buff[j] == '`') {
                                break;
                            } else {
                                temp2.push_back(buff[j]);
                            }
                        }
                        username = temp2;
                        accept_request(groupid, current_user, username, connfd);
                        break;
                    } 
                    else {
                        bzero(buff, MAX);
                        string c = "please login before doing this";
                        int i;
                        for (i = 0; i < c.size(); i++) {
                            buff[i] = c[i];
                        }
                        buff[i] = '\n';
                        write(connfd, buff, sizeof(buff));
                        break;
                    }
                } else if (task == "list_groups") {
                    if (loggedin.find(current_user) != loggedin.end() &&
                        loggedin[current_user] == true) {
                        list_groups(connfd);

                    } else {
                        bzero(buff, MAX);
                        string c = "please login before doing this";
                        int i;
                        for (i = 0; i < c.size(); i++) {
                            buff[i] = c[i];
                        }
                        buff[i] = '\n';
                        cout<<"Abt to send b "<<buff<<":"<<connfd<<endl;
                        write(connfd, buff, sizeof(buff));
                        break;
                    }

                } else if (task == "list_group_users") {
                    if (loggedin.find(current_user) != loggedin.end() &&
                        loggedin[current_user] == true) {
                        for (int j = i + 1; j < sizeof(buff); j++) {
                            if (buff[j] == '`') {
                                groupid = temp2;
                                temp2.clear();
                                // cout << groupid << endl;
                                break;
                            } else {
                                temp2.push_back(buff[j]);
                            }
                        }
                        list_group_users(groupid, connfd);
                        break;
                    } else {
                        bzero(buff, MAX);
                        string c = "please login before doing this";
                        int i;
                        for (i = 0; i < c.size(); i++) {
                            buff[i] = c[i];
                        }
                        buff[i] = '\n';
                        write(connfd, buff, sizeof(buff));
                        break;
                    }
                } else if (task == "upload_file") {
                    
                    if (loggedin.find(current_user) != loggedin.end() && loggedin[current_user] == true)
                     {
                        for (int j = i + 1; j < sizeof(buff); j++) 
                        {  // cout<< j;
                            if (buff[j] == ' ') {
                               // cout << "\nInside IF" << endl;
                                s.push_back(temp2);
                                // cout << temp2;
                                temp2.clear();
                                // cout << username << endl;
                        }   else if (buff[j] == '`') {
                               // cout << "buff[j] : " << buff[j] << endl;
                               // cout << "\nInside ELSE IF " << temp2 << endl;
                                break;

                            }   
                            else {
                               // cout << "\nInside ELSE" << endl;
                                temp2.push_back(buff[j]);
                            }
                        }
                    //  fflush(stdout);
                    //  fflush(stdin);
                       // cout << "Outside LOOP" << endl;
                        file_size = s[4];
                      //  cout << "File SIZE : " << file_size << endl;
                        groupid = s[1];
                      //  cout << "Group id : " << groupid << endl;
                        clientport = s[2];
                      //  cout << clientport << endl;
                        filepath = s[0];
                        clientip = s[3];
                        sha = temp2;
                       // cout << "filepath:" << filepath << endl;
                        string temp3;
                       // cout<<"outside for loop"<<endl;
                        for(int l = filepath.length() - 1; l >= 0; l--)
                        {   
                          //  cout<<"inside loop"<<endl;
                            if(filepath[l] == '/' )
                            {
                                reverse(temp3.begin(),temp3.end());
                                // cout<<temp3<<endl;
                                break;
                            }
                            else{
                                temp3.push_back(filepath[l]);
                            }
                        }
                        
                          if (group.find(groupid) != group.end()) 
                        {
                         if (find(group[groupid].begin(), group[groupid].end(), current_user) != group[groupid].end())
                        {
                        
                        string filename = temp3;
                        vector<string> tempstrings;
                        tempstrings.push_back(filename);
                        tempstrings.push_back(filepath);
                        tempstrings.push_back(file_size);
                        tempstrings.push_back(clientport);
                        tempstrings.push_back(clientip);
                        tempstrings.push_back(current_user);
                        tempstrings.push_back(sha);
                        
                        //client ip
                        //sha
                        //tempstrings[4] = hash;
                        groupfiles[groupid].push_back(tempstrings);
                        bzero(buff, MAX);
                        for (auto i : groupfiles) 
                        {
                           // cout << i.first <<" "<<endl;
                            // for(auto j : i.second)
                            // {
                            //     cout << j[0] <<" " <<j[1] << " " << j[2] << " " << j[3] << " "<< j[4] << " "<<j[5]<<" "<<j[6]<<endl;
                            // }
                        }
                        string c = "file uploaded successfully";
                        int m;
                        for (m = 0; m < c.size(); m++) {
                            buff[m] = c[m];
                        }
                        buff[m] = '\n';
                        write(connfd, buff, sizeof(buff));
                        break;
                        }
                        else{
                            string c = "user not in the group";
                        int m;
                        for (m = 0; m < c.size(); m++) {
                            buff[m] = c[m];
                        }
                        buff[m] = '\n';
                        write(connfd, buff, sizeof(buff));
                        break;
                        }
                        }
                     
                        else{
                            char buff[MAX];
                            bzero(buff, MAX);
                            string c = "group does not exist";
                            int i;
                            for (i = 0; i < c.size(); i++) {
                                buff[i] = c[i];
                            }
                            buff[i] = '\n';
                            write(connfd, buff, sizeof(buff));
                            break;
                        } 
                }
                 else{
                        bzero(buff, MAX);
                        string c = "please login before doing this";
                        int i;
                        for (i = 0; i < c.size(); i++) {
                            buff[i] = c[i];
                        }
                        buff[i] = '\n';
                        write(connfd, buff, sizeof(buff));
                        break;
                 }
            }
            else if(task == "download_file")
            {
                vector<string> s; 
                 if (loggedin.find(current_user) != loggedin.end() && loggedin[current_user] == true) {
                        for (int j = i + 1; j < sizeof(buff); j++) {
                            if (buff[j] == ' ') {
                                s.push_back(temp2);
                                temp2.clear();
                                // cout << username << endl;
                            } 
                            else if (buff[j] == '`') {
                                break;
                            } 
                            else {
                                temp2.push_back(buff[j]);
                            }
                        }
                        bzero(buff, MAX);
                        destination_path = s[2];
                       // cout<<"destination_path:"<<destination_path<<endl;
                        groupid = s[0];
                        //cout<<"groupid"<<groupid<<endl;
                        downloadfilename = s[1]; 
                        //cout<<"downloadfilename:"<<downloadfilename<<endl;
                        clientip = s[3];
                        //cout<<"clientip:"<<clientip<<endl;
                        clientport = temp2;
                        //cout<<"clientport:"<<clientip<<endl;
                        
                        if (group.find(groupid) != group.end()) 
                        {
                            if (find(group[groupid].begin(), group[groupid].end(), current_user) != group[groupid].end())
                            {  // cout<<"before downloading file"<<endl;
                                download_file(groupid, current_user,destination_path,downloadfilename,connfd,clientip,clientport);
                                break;  
                            }
                            else
                            {
                                string c = "user not in the group";
                                int m;
                                for (m = 0; m < c.size(); m++) {
                                    buff[m] = c[m];
                                }
                                buff[m] = '\n';
                                write(connfd, buff, sizeof(buff));
                                break;
                            }
                        }
                         else{
                            char buff[MAX];
                            bzero(buff, MAX);
                            string c = "group does not exist";
                            int i;
                            for (i = 0; i < c.size(); i++) {
                                buff[i] = c[i];
                            }
                            buff[i] = '\n';
                            write(connfd, buff, sizeof(buff));
                            break;
                        } 
                 }
                 else{ 
                        string c = "please login before doing this";
                        int i;
                        for (i = 0; i < c.size(); i++) {
                            buff[i] = c[i];
                        }
                        buff[i] = '\n';
                        write(connfd, buff, sizeof(buff));
                        break;
                 }
            }
                 else if(task == "show_downloads")
                {
                    string sc;
                    // sc.push_back('~');
                    sc+="\n[D]\n[C] ";
                   // cout<<"inside show downloads"<<endl;
                    if (loggedin.find(current_user) != loggedin.end() && loggedin[current_user] == true) {
                        for(auto i : showdw[current_user])
                        {
                           // cout<<"show downloads:::::::::::"<<i.first<<" "<<i.second<<endl; 
                            sc+=(i.first+" "+i.second+" ");
                        }
                        // sc.push_back('`');
                       // cout<<"SDDDDD: "+sc<<endl;
                        string c = sc;
                        int i;
                        bzero(buff,sizeof(buff));
                        for (i = 0; i < c.size(); i++) {
                            buff[i] = c[i];
                        }
                        buff[i] = '\n';
                        //cout<<"bdfjbvdhbvjdbvjebe"<<endl;
                        write(connfd, buff, sizeof(buff));
                        break;
                    }
                    else{
                        string c = "please login before doing this";
                        int i;
                        for (i = 0; i < c.size(); i++) {
                            buff[i] = c[i];
                        }
                        buff[i] = '\n';
                        write(connfd, buff, sizeof(buff));
                        break;
                    }
                }
                else if(task =="list_files")
                {
                     if (loggedin.find(current_user) != loggedin.end() && loggedin[current_user] == true) {
                         for (int j = i + 1; j < sizeof(buff); j++) {
                            if (buff[j] == ' ') {
                                s.push_back(temp2);
                                temp2.clear();
                                // cout << username << endl;
                            } 
                            else if (buff[j] == '`') {
                                break;
                            } 
                            else {
                                temp2.push_back(buff[j]);
                            }
                        }
                        bzero(buff, MAX);
                        string groupid = temp2;
                        string sc;
                        vector<vector<string>> d = groupfiles[groupid];
                        for(int i = 0;i < d.size();i++)
                        {
                            sc+=d[i][0]+" ";
                        }
                        sc+="\n";
                        string c = sc;
                        int i;
                        bzero(buff,sizeof(buff));
                        for (i = 0; i < c.size(); i++) {
                            buff[i] = c[i];
                        }
                        buff[i] = '\n';
                       // cout<<"bdfjbvdhbvjdbvjebe"<<endl;
                        write(connfd, buff, sizeof(buff));
                        break;
                     }
                     else{
                        string c = "please login before doing this";
                        int i;
                        for (i = 0; i < c.size(); i++) {
                            buff[i] = c[i];
                        }
                        buff[i] = '\n';
                        write(connfd, buff, sizeof(buff));
                        break;
                     }
                }
                else if(task == "stop_share")
                {
                     if (loggedin.find(current_user) != loggedin.end() && loggedin[current_user] == true) {
                    for (int j = i + 1; j < sizeof(buff); j++) 
                        {  // cout<< j;
                            if (buff[j] == ' ') {
                                // cout << "\nInside IF" << endl;
                                s.push_back(temp2);
                               // cout << temp2;
                                temp2.clear();
                               // cout << username << endl;
                        }   else if (buff[j] == '`') {
                               // cout << "buff[j] : " << buff[j] << endl;
                                //cout << "\nInside ELSE IF " << temp2 << endl;
                                break;

                            }   
                            else {
                               // cout << "\nInside ELSE" << endl;
                                temp2.push_back(buff[j]);
                            }
                        }
                        string groupid= s[0];
                        string filename = temp2;
                        for( i = 0;i < groupfiles[groupid].size();i++)
                        {       
                        if(groupfiles[groupid][i][0]==filename && groupfiles[groupid][i][5]==current_user)
                        {
                            groupfiles[groupid].erase(groupfiles[groupid].begin() + i);
                            break;
                        }
                     }
                     string c = "No one can download this file now from you";
                        int i;
                        bzero(buff,sizeof(buff));
                        for (i = 0; i < c.size(); i++) {
                            buff[i] = c[i];
                        }
                        buff[i] = '\n';
                        write(connfd, buff, sizeof(buff));
                        break;

                     }
                     else{
                        string c = "please login before doing this";
                        int i;
                        for (i = 0; i < c.size(); i++) {
                            buff[i] = c[i];
                        }
                        buff[i] = '\n';
                        write(connfd, buff, sizeof(buff));
                        break;
                     }

                }
                
            else{
                string c = "enter valid command";
                        int i;
                        for (i = 0; i < c.size(); i++) {
                            buff[i] = c[i];
                        }
                        buff[i] = '\n';
                        write(connfd, buff, sizeof(buff));
                        break;
            }
            } 
            else {
                temp.push_back(buff[i]);
            }
        }
    }
    return 0;
}
void * listen(void *arguments)
{     struct arg_struct *args = (struct arg_struct *)arguments;
       struct sockaddr_in cli = args->arg1;
       int sockfd = args->arg2;
        socklen_t len = sizeof(cli);
       int connfd;

    while (1) {
        connfd = accept(sockfd, (SA *)&cli, &len);
        if (connfd < 0) {
            printf("server accept failed...\n");
            exit(0);
        } else
            printf("server accept the client...\n");
        // pthread_t t;
        // int p;
        int *pclient = new int;
        *pclient = connfd;

        pthread_create(&ttid[threads_counter++], NULL, whatto, pclient);
    }

    for (int i = 0; i <= threads_counter; i++) {
        pthread_join(ttid[i], NULL);
    }
    close(sockfd);
   
}
void conn(void) {
   // cout << "mff";
    int n;
    string s;
    s = one;
    n = stoi(two);
   // cout << n;
    ifstream myfile(s);
    string number, sport, serverip;
    if (n == 1) {
        myfile >> number;
        char *token = strtok(number.data(), ":");
        serverip = token;
        token = strtok(NULL, ":");
        sport = token;
      //  cout << "hgn";

    } else {
        myfile >> number;
        myfile >> number;
        char *token = strtok(number.data(), ":");
        serverip = token;
        token = strtok(NULL, ":");
        sport = token;
    }
    int port = stoi(sport);
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    } else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
     servaddr.sin_addr.s_addr = inet_addr(serverip.c_str());
    servaddr.sin_port = htons(port);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA *)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    } else
        printf("Socket successfully binded..\n");

    // Now server is ready to listen and verification

    if ((listen(sockfd, 10)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    } else
        printf("Server listening..\n");

   

    // Accept the data packet from client and verification
    pthread_t some_thread;
    args =(struct arg_struct *)malloc(sizeof(struct arg_struct) * 1);
    args->arg1 = cli;
    args->arg2 = sockfd;
    pthread_create(&some_thread, NULL, &listen, args);
    
    while(1)
    {
        string s;
        getline(cin, s);
        if(s == "quit")
        {
            exit(0);
        }
    }
    
}


int main(int argc, char **argv) {
    one = argv[1];
    two = argv[2];
    // string line;
    // string key;
    // string value;
    // paramFile.open("create_user.txt");
    // while (paramFile >> key >> value) {
    //     users[key] = value;  // input them into the map
    // }

    // for (auto i : users) {
    //     cout << i.first << "         " << i.second << endl;
    // }

    conn();
   
}