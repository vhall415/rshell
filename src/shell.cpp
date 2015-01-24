#include <iostream>
#include <string>
#include <vector>
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
string getCmd(string &line)
{
	int i = 0;
	string temp = "";
	while((line[i] == ' ' || line[i] == '\t') && i < line.size())   //loop until not a space or tab
		i++;
	while(i < line.size() && !(line[i] == ' ' || line[i] == '\t' || line[i] == '&' || line[i] == ';' || line[i] == '|'))    //determine the cmd
	{
		temp += line[i];
		i++;
	}
	if(i < line.size())
		line = line.substr(i+1, line.size());
	else
		line = "";
	return temp;
}
/*
vector<int> connect(string line)
{
	vector<int> index;
	std::string::size_type n, m = 0;
	bool b = true;
	string temp = line;
	while(b)	//semi counter
	{
		n = temp.find(';');
		if(!(n == std::string::npos))
		{	
			m += n;
			index.push_back(m);
			temp = temp.substr(n+1);
		}
		else
			b = false;
	}
	temp = line;
	while(b)	//andd counter
	{
		n = temp.find("&&");
		if(!(n == std::string::npos))
		{
			m += n;
			index.push_back(m);
			temp = temp.substr(n+1);
		}
		else
			b = false;
	}
	temp = line;
	while(b)	//orr counter
	{
		n = temp.find("||");
		if(!(n == std::string::npos))
		{
			m += n;
			index.push_back(m);
			temp = temp.substr(n+1);
		}
		else
			b = false;
	}
	int tmp;
	for(int i = 0; i < index.size(); i++)	//order the index values
	{
		tmp = index[i];
		for(int j = i+1; j < index.size(); i++)
		{
			if(index[j] < tmp)
			{
				tmp = index[j];
				index[j] = index[i];
				index[i] = tmp;
			}
		}
	}
	return index;
}
*/
int getNumParams(int t, string line)
{
	int num = 0;
	while(t < line.size())  //loop to find number of params
	{
		if(line[t] == '&' || line[t] == ';' || line[t] == '|')    //if char is a conjunction
			break; 
		if(!(line[t] == ' ' || line[t] == '\t' || line[t] == '&' || line[t] == ';' || line[t] == '|') && line[t+1] == ' ' || line[t+1] == '\0')
			num++;    //increment if there is a space followed by a char 
		t++;
	}   
	return num;
}   

char** getParams(int num, string line)
{
	cout << num;
	int i = 0;
	char *p[num+1];
	string temp = "";
	for(int x = 0; x < num; x++)
	{
		while(i < line.size() && !(line[i] == ' ' || line[i] == '\t' || line[i] == '&' || line[i] == ';' || line[i] == '|'))
		{
			temp += line[i];
			i++; 
		}
		i++;
		char *c = toChar(temp);
		p[x] = c;   //store param in vector
		cout << ", " << temp;
		temp = "";  //reset temp
	}
	//*p[num] = NULL;
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
	vector<string> cLines, cmds;
	string cmd = "";
	char **params;
	int numParams = 0;
	int i = 0;	//index for line
	string line;
	while(cmd != "exit")
	{	
		//do prompt
		cout << "$ ";	//print command prompt
		getline(cin, line);	//puts the user text in line
		std::string::size_type n = line.find('#');
		if(!(n == std::string::npos))
			line = line.substr(0, n);
		cmd = getCmd(line);
		if(cmd == "exit")
			break;
		cout << "cmd: " << cmd << endl;
		if(line == "")
			numParams == 0;
		else
			numParams = getNumParams(0, line);
		cout << "params: ";
		if(numParams > 0)
		{
			params = getParams(numParams, line);
		}
		else
		{
			params = NULL;
			cout << "none";
		}
		cout << endl;
		char *c = toChar(cmd);
		execute(c, params);
		//separate cmd lines
/*		string tmp;
		vector<int> connectors = connect(line);
		int x = 0;
		while(x < connectors[connectors.size() - 1])
		{
			tmp = line.substr(x, connectors[x]);
			cLines.push_back(tmp);
			x = connectors[x] + 1;
		}
		x = 0;

		while(x < cLines.size())	//loop to get each line cmd and params
		{
			tmp = cLines[x];
			cmds[x] = getCmd(0, tmp);
			if(tmp.size() > 0)
				numParams = getNumParams(0, tmp);
			if(numParams > 0)
				params[x] = getParams(numParams, 0, tmp);
			x++;
		}
		*/
	}
	return 0;
}
