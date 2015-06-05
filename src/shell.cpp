#include <iostream>
#include <cstring>
#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>
#include <errno.h>
#include <unistd.h>
#include <cstdio>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>

using namespace std;



char* getUserInfo(char* user, string &currHost) {
	struct passwd *pass = getpwuid(getuid());
	user = pass->pw_name;
	char host[100];
	gethostname(host, sizeof(host));
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
		return var;
	}

	return -1;
}

int main() {
	vector<string> cmdline;
	//get user info
	char *user;
	string host;
	user = getUserInfo(user, host);
	
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
