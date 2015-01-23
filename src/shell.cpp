#include <iostream>
#include <string>
#include <array>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


using namespace std;

string getCmd(int i, string line)
{
	string cmd = "";
	if(line[i] == ' ' || line[i] == '\t')   //line starts with space/tab
	{
		while((line[i] == ' ' || line[i] == '\t') && i < line.size())   //loop until not a space or tab
			i++;
	}
	while(i < line.size() && !(line[i] == ' ' || line[i] == '\t' || line[i] == '&' || line[i] == ';' || line[i] == '|'))    //determine the cmd
	{
		if(line[i] == '#')
			break;
		cmd += line[i];
		i++;
	}
	return cmd;
}


int getNumParams(int i, string line)
{
	int num = 0;
	while(t < line.size() - 1)  //loop to find number of params
	{
		if(line[t] == '&' || line[t] == ';' || line[t] == '|' || line[t] == '#')    //if char is a conjunction or a comment
			break; 
		if(line[t] == ' ' && !(line[t+1] == ' ' || line[t+1] == '\t' || line[t+1] == '&' || line[t+1] == ';' || line[t+1] == '|'))
			num++;    //increment if there is a space followed by a char 
	}   
	return num;
}   


void getParams(array<string, numParams> &params, int numParams, int i, string line)
{
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
		params[x] = temp;   //store param in array
		temp = "";  //reset temp
	}
}

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
		cmd = getCmd(i, line);
		int t = i;
		numParams = getNumParams(t, line);
		array<string, numParams> params;	//array to hold all the params
		getParams(&params, numParams, i, line)

		int pid = fork();
		if(pid == -1)	//error
		{
			perror("error in fork");
			exit(1);
		}
		else if(pid == 0)	//in child
		{
			if(-1 == execvp(cmd, params))	//if execvp fails
			{
				perror("error in execvp");
				exit(1);
			}
		}
		else	//in parent
		{
			if(-1 == wait())	//wait for child to finish
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
		out << "cmd: " << cmd << endl << "params: " << params << endl;
		cmd = "exit";
	}
	return 0;
}
