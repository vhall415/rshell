#include <iostream>
#include <string>
#include <array>

using namespace std;

int main()
{
	string cmd = "";
	while(cmd != "exit")
	{
		string line;
		int numParams;
		cout << "$";	//print command prompt
		getline(cin, line);	//puts the user text in line
		int i = 0;	//index for line
		if(line[i] == ' ' || line[i] == '\t')	//line starts with space/tab
		{
			while((line[i] == ' ' || line[i] == '\t') && i < line.size())	//loop until not a space or tab
			i++;
		}
		while(i < line.size() && !(line[i] == ' ' || line[i] == '\t' || line[i] == '&' || line[i] == ';' || line[i] == '|'))	//determine the cmd
		{
			if(line[i] == '#')
				break;
			cmd += line[i];
				i++;
		}
		int t = i;
		while(t < line.size() - 1)	//loop to find number of params
		{
			if(line[t] == '&' || line[t] == ';' || line[t] == '|' || line[t] == '#')	//if char is a conjunction or a comment
				break;
			if(line[t] == ' ' && !(line[t+1] == ' ' || line[t+1] == '\t' || line[t+1] == '&' || line[t+1] == ';' || line[t+1] == '|'))
				numParams++;	//increment if there is a space followed by a char
		}
		array<string, numParams> params;	//array to hold all the params
		string temp = "";
		for(int x = 0; x < numParams; x++)
		{
			while(i < line.size() && !(line[i] == ' ' || line[i] == '\t' || line[i] == '&' || line[i] == ';' || line[i] == '|'))
			{
				if(line[i] == '#')
					break;
				temp += line[i];
				i++;
			}
			params[x] = temp;	//store param in array
			temp = "";	//reset temp
		}
		int pid = fork();
		if(pid == -1)	//error
		{
			perror("error in fork");
			exit(1);
		}
		else if(pid == 0)	//in child
		{
			int e = execvp(cmd, params);
			if(e == -1)
			{
				perror("error in execvp");
				exit(1);
			}
		}
		else	//in parent
		{
			int w = wait();	//wait for child to finish
			if(w == -1)
			{
				perror("error in wait");
				exit(1);
			}
		}
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
		out << cmd << endl;
		cmd = "exit";
	}
	return 0;
}
