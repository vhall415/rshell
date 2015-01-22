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
		while(i < line.size() && !(line[i] == ' ' || line[i] == '\t' || line[i] == '&' || line[i] == ';' || line[i] == '|'))
		{
			if(line[i] == '#')
				break;
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
