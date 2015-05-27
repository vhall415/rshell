#include <iostream>
#include <cstring>
#include <vector>
#include <string>
#include <string.h>
#include <boost/algorithm/string.hpp>
#include <errno.h>
#include <unistd.h>
#include <cstdio>
#include <stdlib.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <pwd.h>
#include <fcntl.h>
#include <algorithm>

#define outlist O_RDWR | O_CREAT | O_TRUNC, 00744 
#define inlist O_RDONLY 
#define appendlist O_RDWR | O_CREAT | O_APPEND, 00744 
#define num0appendlist O_RDONLY | O_CREAT | O_APPEND, 00744 
#define num0outlist O_RDONLY | O_TRUNC 

using namespace std;

char* getUserInfo(string &currHost) {
	struct passwd *pass = getpwuid(getuid());
	if(pass == NULL)
		perror("getpwuid failed");
	char *user = pass->pw_name;
	char host[100];
	if(-1 == gethostname(host, sizeof(host)))
		perror("gethostname failed");
	currHost = host;
	if(currHost.find('.') != std::string::npos)
		currHost.resize(currHost.find('.'));
	return user;
}

void prompt(const char *user, const string host) {
	cout << user << '@' << host << ' ';
	cout << '$' << ' ';
}

string clean(string cmd) {
	if(!cmd.empty()) {
		//delete starting comments/whitespace
		while((cmd.at(0) == ';' || cmd.at(0) == ' ' || cmd.at(0) == '\t' || cmd.at(0) == '|' || cmd.at(0) == '&')) {
			cmd.erase(0,1);
			if(cmd.empty())
				break;
		}
	}
	//check no duplicate
	if(!cmd.empty()) {
		//delete end comment/whitespace
		while((cmd.at(cmd.size() - 1) == ';' || cmd.at(cmd.size() - 1) == ' ' || cmd.at(cmd.size() - 1) == '\t' || cmd.at(cmd.size() - 1) == '|' || cmd.at(cmd.size() - 1) == '&')) {
			cmd.resize(cmd.size() - 1);
			if(cmd.empty())
				break;
		}
	}
	
	return cmd;
}

string spacing(string cmd) {
	for(unsigned int i = 1; i < cmd.size(); i++) {
		if(cmd.at(i) == ';') {
			if(i + 1 < cmd.size() && cmd.at(i + 1) != ' ')
				cmd.insert(i + 1, " ");
			if(cmd.at(i - 1) != '\\' && cmd.at(i - 1) != ' ')
				cmd.insert(i, " ");
		}

		else if(cmd.at(i - 1) == '&' && cmd.at(i) == '&') {
			if(i + 1 < cmd.size() && cmd.at(i + 1) != ' ')
				cmd.insert(i + 1, " ");
			if(i - 2 >= 0 && cmd.at(i - 2) != ' ')
				cmd.insert(i - 1, " ");
		}
		
		else if(cmd.at(i-1) == '|' && cmd.at(i) == '|'){
			if(i+1 < cmd.size() && cmd.at(i+1) != ' '){
				cmd.insert(i+1, " ");
			}
			if(i-2 >= 0 && cmd.at(i-2) != ' '){
				cmd.insert(i-1, " ");
			}
		}
		else if(cmd.at(i) == '<' && (i+2 < cmd.size() && cmd.at(i+2) == '<' && cmd.at(i+1) == '<')){
			if(i+3 < cmd.size() && cmd.at(i+3) != ' ')
				cmd.insert(i+3, " ");
				if(cmd.at(i-1) != ' '){
					cmd.insert(i, " ");
					i++;
				}
				i += 2;
		}
		else if(cmd.at(i) == '<'){
			if(i+1 < cmd.size() && cmd.at(i+1) != ' '){
				cmd.insert(i+1, " ");
			}
			if(cmd.at(i-1) != ' '){
				cmd.insert(i, " ");
			}
		}
		else if(cmd.at(i) == '>'){
			if(i+1 < cmd.size() && cmd.at(i+1) == '>'){
				if(i+2 < cmd.size() && cmd.at(i+2) != ' '){
					cmd.insert(i+2, " ");
				}
				if(cmd.at(i-1) != ' '){
					cmd.insert(i, " ");
				}
			}
			if(i+1 < cmd.size() && cmd.at(i+1) != '>' && cmd.at(i-1) != '>'){
				if(cmd.at(i+1) != ' '){
					cmd.insert(i+1, " ");
				}
				if(cmd.at(i-1) != ' '){
					cmd.insert(i, " ");
				}
			}
			if(cmd.at(i-1) != ' ' && cmd.at(i-1) != '>'){
				cmd.insert(i, " ");
			}
		}
		else if(cmd.at(i) == '|'){
			if(i+1 < cmd.size() && cmd.at(i+1) != '|' && cmd.at(i-1) != '|'){
				if(cmd.at(i+1) != ' '){
					cmd.insert(i+1, " ");
				}
				if(cmd.at(i-1) != ' '){
					cmd.insert(i, " ");
				}
			}
		}
	}

	return cmd;
}

vector<char*> vecStrToChar(vector<string> s) {
	vector<char*> c;
	
	string tmp;
	for(unsigned int i = 0; i < s.size(); i++) {
		tmp = s.at(i);
		char *let = new char[tmp.size() + 1];
		c.push_back(strcpy(let, tmp.c_str()));
	}

	c.push_back(NULL);
	return c;
}
bool isNum(const string &s) {
	string::const_iterator it = s.begin();
	while(it != s.end() && isdigit(*it))
		it++;
		return !s.empty() && it == s.end();
}

void rmVec(vector<char*> v) {
	for(unsigned int i = 0; i < v.size(); i++)
		delete [] v.at(i);
}

int exec(string cmds) {
	vector<string> vec;
	cmds = clean(cmds);
	int var;

	if(cmds.size() > 0) {
		cmds.c_str();
		boost::split(vec, cmds, boost::is_any_of(" "), boost::token_compress_on);
		if(vec.at(0) == "exit" || vec.at(0) == "EXIT")
			exit(1);
	}

	vector<char*> c = vecStrToChar(vec);
	char *args[300];
	
	for(unsigned int i = 0; i < c.size(); i++)
		args[i] = c.at(i);
	
	int pid = fork();
	if(pid == -1) {
		perror("Error with fork");
		exit(1);
	}
	else if(pid == 0) {
		int x = execvp(args[0], args);
		if(x == -1) {
			perror("Error with execvp");
			exit(1);
		}
	}
	else if(pid != 0) {
		while(wait(&var) != pid)
			perror("Error with wait");
		for(unsigned int i = 0; i < c.size(); i++)
			delete [] c.at(i);
		return var;
	}
	
	for(unsigned int i = 0; i < c.size(); i++)
		delete [] c.at(i);
	return -1;
}

int execredir(string left, string right, int dupval, int id){
	int var;
	right = clean(right);
	left = clean(left);
	vector<string> vec;
	left.c_str();
	boost::split(vec, left, boost::is_any_of(" "), boost::token_compress_on);
	vector<char*> v = vecStrToChar(vec);	
	char *args[300]; 
	for(unsigned i = 0; i < v.size(); ++i){
		args[i] = v.at(i);
	}
	int fd = -1;
	if(id == 0){
		fd = open(right.c_str(), inlist); 
		if(fd == -1){
			perror("Error with open");
			rmVec(v);
			return(-1);
		}
	}
	if(id == 1 || id == 4){
		fd = open(right.c_str(), outlist); 
		if(fd == -1){
			perror("Error with open");
			rmVec(v);
			return(-1);
		}
	}
	if(id == 2 || id == 6){
		fd = open(right.c_str(), appendlist); 
		if(fd == -1){
			perror("Error with open");
			rmVec(v);
			return(-1);
		}
	}
	if(id == 3){
		fd = open(right.c_str(), num0outlist);
		if(fd == -1){
			perror("Error with open");
			rmVec(v);
			return(-1);
		}
	}
	if(id == 5){
		fd = open(right.c_str(), num0appendlist);
		if(fd == -1){
			perror("Error with open");
			rmVec(v);
			return(-1);
		}
	}
	int retstd = dup(dupval);
	if(retstd == -1){
		perror("Error with dup");
		rmVec(v);
		return(-1);
	}

	int pid = fork();
	if(pid == -1){
		perror("Error with fork");
		exit(1);
	}
	else if(pid == 0){
		if(-1 == close(dupval)){
			perror("Error with close");
			exit(1);
		}
		if(-1 == dup(fd)){
			perror("Error dup");
			exit(1);
		}
		execvp(args[0], args);
		perror("Execvp failed");
		exit(1);
	}
	else if(pid != 0){
		while(wait(&var) != pid)
			perror("Error with wait");
			if(-1 == close(fd)){
				perror("Error with close");
				exit(1);
			}
			if(-1 == dup(retstd)){
				perror("Error with dup");
				exit(1);
			}
			rmVec(v);
			return var;
	}
	rmVec(v);
	return -1;
}

int execredir2(string left, string middle, string right, int dupval){
	int var;
	right = clean(right);
	middle = clean(middle);
	left = clean(left);
	vector<string> vec;
	left.c_str();
	boost::split(vec, left, boost::is_any_of(" "),	
	boost::token_compress_on);
	vector<char*> v = vecStrToChar(vec);	
	char *args[300]; 
	for(unsigned int i = 0; i < v.size(); i++){
		args[i] = v.at(i);
	}
	int fd1;
	if(dupval == 0){
		fd1 = open(right.c_str(), outlist); 
		if(fd1 == -1){
			perror("Error with open");
			rmVec(v);
			return(-1);
		}
	}
	if(dupval == 1){
		fd1 = open(right.c_str(), appendlist);
		if(fd1 == -1){
			perror("Error with open");
			rmVec(v);
			return(-1);
		}	
	}
	int fd2 = open(middle.c_str(), O_RDONLY); 
	if(fd2 == -1){
		perror("Error with open");
		rmVec(v);
		return(-1);
	}

	int retstdin = dup(0);
	if(retstdin == -1){
		perror("Error with dup");
		rmVec(v);
		return(-1);
	}

	int retstdout = dup(1);
	if(retstdout == -1){
		perror("Error with dup");
		rmVec(v);
		return(-1);
	}

	int pid = fork();
	if(pid == -1){
		perror("Error with fork");
		exit(1);
	}
	else if(pid == 0){
		if(-1 == close(1)){
			perror("Error with close");
			exit(1);
		}
		if(-1 == dup(fd1)){
			perror("Error with dup");
			exit(1);
		}
		if(-1 == close(0)){
			perror("Error with close");
			exit(1);
		}
		if(-1 == dup(fd2)){
			perror("Error with dup");
			exit(1);
		}

		execvp(args[0], args);
		perror("Exec failed");
		exit(1);
	}
	else if(pid != 0){
		while(wait(&var) != pid)
			perror("Error with wait");
			if(-1 == close(fd1)){
				perror("Error with close");
				exit(1);
			}
			if(-1 == dup(retstdout)){
				perror("Error with dup");
				exit(1);
			}
			if(-1 == close(fd2)){
				perror("Error with close");
				exit(1);
			}
			if(-1 == dup(retstdin)){
				perror("Error with dup");
				exit(1);
			}
			rmVec(v);
			return var;
	}
	rmVec(v);
	return -1;
}

int execRedirection(vector<string> cmds){
	int rval = -1;
	bool in = false, in3 = false, out = false, appends = false, 
	numOutDetect = false, numAppendDetect = false;
	int numOut, numAppend;
	for(unsigned int i = 0; i < cmds.size(); i++){
		if(cmds.at(i) == "<")
			in = true;
			if(cmds.at(i) == "<<<")
				in3 = true;
				if(cmds.at(i) == ">")
					out = true;
					if(cmds.at(i) == ">>")
						appends = true;
						if(isNum(cmds.at(i)) && (i+1 < cmds.size() && cmds.at(i+1) == ">" && !numOutDetect)){
							numOutDetect = true;
							numOut = atoi(cmds.at(i).c_str());
						}
						if(isNum(cmds.at(i)) && (i+1 < cmds.size() && cmds.at(i+1) == ">>" && !numAppendDetect)){
							numAppendDetect = true;
							numAppend = atoi(cmds.at(i).c_str());
						}
	}	
	if((in || in3) && (out || appends)){
		unsigned inn = 0, inn2 = 0, outt = 0, appendd = 0;
		cmds.push_back(";");
		string firstExec = "", lastExec = "", midExec = "";
		int lastcmd = 0;
		int id = 1;
		bool infirst;
		for(unsigned int i = 0; i < cmds.size(); i++){
			if(isNum(cmds.at(i)) && i+1 < cmds.size() && (cmds.at(i+1) == ">>" || cmds.at(i+1) == ">"));
			else if(cmds.at(i) == "<" || cmds.at(i) == ">" || cmds.at(i) == ">>" || cmds.at(i) == "<<<" || cmds.at(i) == ";"){
				if(lastExec != ""){
					if(infirst){
						if(inn2 == 1){
							if(midExec.at(0) == '"' && midExec.at(midExec.size()-2) == '"')
									midExec = midExec.substr(1, midExec.size()-3);
									lastExec = "echo " + midExec;
									rval = execredir(lastExec, firstExec, 1, id);
						}
						else
							rval = execredir2(lastExec, midExec, firstExec, lastcmd);
					}
					else{
						rval = execredir2(lastExec, firstExec, midExec, lastcmd);
					}
					lastExec = "";
				}
				else if(midExec != ""){
					lastExec = midExec;
					midExec = firstExec;
				}
				else
					midExec = firstExec;
					if((inn == 1 || inn2 == 1) &&(outt == 0 && appendd == 0))
						infirst = true;
						else if ((outt == 1 || appendd == 1) && (inn == 0 && inn2 == 0))
							infirst = false;
							if(cmds.at(i) == "<"){
								if(inn == 1 || inn2 == 1){
									cerr << "Error: One input allowed. Only first was executed." << endl;
									break;
								}
								inn++;
							}
							else if(cmds.at(i) == "<<<"){
								if(inn == 1 || inn2 == 1){
									cerr << "Error: One input allowed. Only first was executed." << endl;
									break;
								}
								inn2++;
							}
							else if(cmds.at(i) == ">"){
								if(outt == 1 || appendd == 1){
									cerr << "Error: One output allowed. Only first was executed." << endl;
									break;
								}
								outt++;
							}
							else if(cmds.at(i) == ">>"){
								id = 2;
								lastcmd = 1;
								if(outt == 1 || appendd == 1){
									cerr << "Error: One output allowed. Only first was executed." << endl;
									break;
								}
								appendd++;
							}
							else
								break;
								firstExec = "";		
			}
			else{
				firstExec.append(cmds.at(i));
				firstExec.append(" ");
			}
		}

	}
	else if(in || in3 || out || appends){
		unsigned inn = 0, inn2 = 0, outt = 0, appendd = 0;
		cmds.push_back(";");
		string firstExec = "", lastExec = "";
		int lastcmd = 0;
		for(unsigned i = 0; i < cmds.size(); ++i){
			if(isNum(cmds.at(i)) && i+1 < cmds.size() && (cmds.at(i+1) == ">>" || cmds.at(i+1) == ">"));
			else if(cmds.at(i) == "<" || cmds.at(i) == ">" || cmds.at(i) == ">>" || cmds.at(i) == "<<<" || cmds.at(i) == ";"){
				if(lastcmd == 0){
					if(lastExec != ""){
						rval = execredir(lastExec, firstExec, 0, 0);
						lastExec = "";
					}
					else
						lastExec = firstExec;
				}
				else if(lastcmd == 1){
					if(lastExec != ""){
						if(numOutDetect){
							if(numOut == 0)
									rval = execredir(lastExec, firstExec, numOut, 3);
									else
											rval = execredir(lastExec, firstExec, numOut, 4);
						}
						else
							rval = execredir(lastExec, firstExec, 1, 1);
							lastExec = "";
					}
					else
						lastExec = firstExec;
				}
				else if(lastcmd == 2){
					if(lastExec != ""){
						if(numAppendDetect){
							if(numAppend == 0)
									rval = execredir(lastExec, firstExec, numAppend, 5);
									else
											rval = execredir(lastExec, firstExec, numAppend, 6);
						}
						else
							rval = execredir(lastExec, firstExec, 1, 2);
							lastExec = "";
					}
					else
						lastExec = firstExec;
				}
				else if (lastcmd == 3){
					if(lastExec != ""){
						if(firstExec.at(0) == '"' && firstExec.at(firstExec.size()-2) == '"')
							cout << firstExec.substr(1, firstExec.size()-3) << endl;
							else
								cout << firstExec << endl;
								rval = 0;
								lastExec = "";
					}
					else
						lastExec = firstExec;
				}
				if(cmds.at(i) == "<"){
					lastcmd = 0;
					if(inn == 1 || inn2 == 1){
						cerr << "Error: One input allowed. Only first was executed." << endl;
						break;
					}
					inn++;
				}
				else if(cmds.at(i) == ">"){
					lastcmd = 1;
					if(outt == 1 || appendd == 1){
						cerr << "Error: One output allowed. Only first was executed." << endl;
						break;
					}
					outt++;
				}
				else if(cmds.at(i) == ">>"){
					lastcmd = 2;
					if(outt == 1 || appendd == 1){
						cerr << "Error: One output allowed. Only first was executed." << endl;
						break;
					}
					appendd++;
				}
				else if(cmds.at(i) == "<<<"){
					lastcmd = 3;
					if(inn == 1 || inn2 == 1){
						cerr << "Error: One input allowed. Only first was executed." << endl;
						break;
					}
					inn2++;
				}
				else
					break;
					firstExec = "";		
			}
			else{
				firstExec.append(cmds.at(i));
				firstExec.append(" ");
			}
		}
	}
	return rval;
}

void handler(int i){
	if(i == SIGINT){
		int temp = getpid();
		if(-1 == temp){
			perror("getpid");
			exit(1);
		}
		if(temp == 0)
			exit(0);
		return;
	}
	
	if(i == SIGTSTP){
		raise(SIGSTOP);
		cout << "  Process Stopped" << endl;
	}
}

int foreground(){
	int status = 0;
	int succ = kill(0,SIGCONT);
	if(succ == -1){
		perror("kill");
	}
	else {
		while(errno != ECHILD && errno != EINTR){
			waitpid(0,&status,WCONTINUED);
			if(status != 0){
				perror("execv");
				succ = status;
			}
		}
	}
	return (succ == 0);
}

void background(){
	int succ = kill(0,SIGCONT);
	if(succ == -1){
		perror("kill");
	}
}

int main() {
	if(SIG_ERR == signal(SIGINT, handler)){
		perror("Signal Error");
		exit(1);
	}
	if(SIG_ERR == signal(SIGTSTP, handler)){
		perror("Signal Error");
		exit(1);
	}
	
	vector<string> cmdline;
	//get user info
	char *user;
	string host;
	user = getUserInfo(host);
	
	while(1) {
		//output prompt
		prompt(user, host);

		//get command line
		string cmd = "";
		getline(cin, cmd);


		if(cin.fail()) {
			cout << "Error: cin" << endl;
			return 0;
		}

		//ignore comments
		if(cmd.find('#') != std::string::npos)
			cmd.resize(cmd.find('#'));

		cmd = clean(cmd);

		//process commands if not just spaces or comments
		if(cmd.size() > 0) {
			cmd = spacing(cmd);
			cmd.c_str();
			
			//parse command line by spaces and tabs
			boost::split(cmdline, cmd, boost::is_any_of(" , \t"), boost::token_compress_on);
			cmdline.push_back(";");
			int cond;
			string exe = "";
			bool pass = true;
			int lastcmd = 0;	//0 = none, 1 = &&, 2 = ||

			for(unsigned int i = 0; i < cmdline.size(); i++) {
				//do nothing if 1st cmd is && or ||
				if(exe == "" && (cmdline.at(i) == "&&" || cmdline.at(i) == "||"));

				//&&, || is found between cmds
				else if(cmdline.at(i) == "&&" || cmdline.at(i) == "||" || cmdline.at(i) == ";") {
					if(lastcmd == 0) {
						cond = exec(exe);
						if(cond == 0)
							pass = true;
						else
							pass = false;
					}
					//last is &&
					else if(lastcmd == 1) {
						if(pass) {
							cond = exec(exe);
							if(cond == 0)
								pass = true;
							else
								pass = false;
						}
					}
					//last is ||
					else if(lastcmd == 2) {
						if(!pass) {
							cond = exec(exe);
							if(cond == 0)
								pass = true;
							else
								pass = false;
						}
					}
					//set last cmd op
					if(cmdline.at(i) == ";")
						lastcmd = 0;
					else if(cmdline.at(i) == "&&")
						lastcmd = 1;
					else if(cmdline.at(i) == "||")
						lastcmd = 2;

					exe = "";
				}
				//normal command, add to exe
				else {
					exe.append(cmdline.at(i));
					exe.append(" ");
				}
			}
		}
	}

	return 0;
}
