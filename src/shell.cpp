#include <iostream>
#include <string>
#include <array>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


using namespace std;

char* toChar(string s)	//convert string to char*
{
	char c[s.size() + 1];
	for(int i = 0; i < s.size(); i++)
		c[i] = s[i];
	c[s.size()] = 0;
	return c;
}
/*
string toString(char *c)	//convert char* to string
{
	cout << "c start: " << *c << endl;
	string s = "";
	while(!(*c == '\0'))
	{
	cout << "c start: " << *c << endl;
		cout << "c: " << *c << endl;
		c++;
	}
	return s;
}
*/
string getCmd(int i, string line)
{
	string temp = "";
	if(line[i] == ' ' || line[i] == '\t')   //line starts with space/tab
	{
		while((line[i] == ' ' || line[i] == '\t') && i < line.size())   //loop until not a space or tab
			i++;
	}
	while(i < line.size() && !(line[i] == ' ' || line[i] == '\t' || line[i] == '&' || line[i] == ';' || line[i] == '|'))    //determine the cmd
	{
		temp += line[i];
		i++;
	}
	return temp;
}

int getNumParams(int t, string line)
{
	int num = 0;
	while(t < line.size() - 1)  //loop to find number of params
	{
		if(line[t] == '&' || line[t] == ';' || line[t] == '|')    //if char is a conjunction
			break; 
		if(line[t] == ' ' && !(line[t+1] == ' ' || line[t+1] == '\t' || line[t+1] == '&' || line[t+1] == ';' || line[t+1] == '|'))
			num++;    //increment if there is a space followed by a char 
		t++;
	}   
	return num;
}   

char** getParams(int num, int i, string line)
{
	char *p[num];
	string temp = "";
	for(int x = 0; x < num; x++)
	{
		while(i < line.size() && !(line[i] == ' ' || line[i] == '\t' || line[i] == '&' || line[i] == ';' || line[i] == '|'))
		{
			temp += line[i];
			i++; 
		}
		char *s = toChar(temp);
		p[x] = s;   //store param in array
		temp = "";  //reset temp
	}
	return p;
}

void execute(char *cmd, char **params)
{
	int pid = fork();
	{
		if(pid == -1)	//error
		{
			perror("error in fork");
			exit(1);
		}
		else if(pid == 0)	//in child
		{
			if(-1 == execvp(cmd, params))
				perror("error in execvp");
			exit(1);
		}
		else	//in parent
		{
			if(-1 == wait(0))	//wait for child to finish
				perror("error in wait");
			exit(1);
		}
	}
}

int main()
{
	string cmd = "";
	int i = 0;	//index for line
	string line;
	//do first prompt
	cout << "$ ";	//print command prompt
	getline(cin, line);	//puts the user text in line
	std::string::size_type n = line.find('#');
	if(!(n == std::string::npos))
		line = line.substr(0, n);
	if(i < line.size())
		cmd = getCmd(i, line);
	while(cmd != "exit")
	{
		int t = i;
		int numParams;
		if(t < line.size())
			numParams = getNumParams(t, line);
		char **params;
		if(i < line.size())
			params = getParams(numParams, i, line);	//array to hold all the params
		char *c = toChar(cmd);
		//execute(c, params);
		/*
		if(i < line.size() && line[i] == '&')
		{
			if(!(line[i+1] == '&'))
				break;
		}
		if(i < line.size() && line[i] == '|')
		{
			if(!(line[i+1] == '|'))
				break;
		}
		if(i < line.size() && line[i] == ';')
		{
			
		}
		*/
		//cout << line << endl;
		//cout << "cmd: " << b << endl;; // << "params: " << params << endl;
		//cmd = toChar("exit");
		cout << "$ ";	//print command prompt
		getline(cin, line);	//puts the user text in line
		std::string::size_type n = line.find('#');
		if(!(n == std::string::npos))
			line = line.substr(0, n);
		if(i < line.size())
			cmd = getCmd(i, line);
	}
	return 0;
}
