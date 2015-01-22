#include <iostream>

using namespace std;

int main()
{
	char *cmd = "";
	while(cmd != "exit")
	{
		char *line;
		cout << "$";	//print command prompt
		getline(cin, line);	//puts the user text in line
		int i = 0;	//index for line
		if(line[i] == " " || line[i] == "\t")	//line starts with space/tab
		{
		while(line[i] == " " || line[i] == "\t")	//loop until not a space or tab
		i++;
		}
		if(line[i] == "#")
			break;
		while(line[i] != " " || line[i] != "\t" || line[i] != "&&" || line[i] != ";" || line[i] != "||")
		{
			cmd += line[i];
			i++;
		}
		/*
		while(i < line.size())	
		{
		
		}
		*/
		cout << cmd << endl;
		cmd = "exit";
	}
	return 0;
}
