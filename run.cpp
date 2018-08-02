#include <iostream>
#include <unistd.h>//system constants like
#include <fcntl.h>//for file descriptors and open system call
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include<stdlib.h>
#include <string>
#include<cstdint>
#include <stdexcept>
#include <cctype>
#include <vector>

//unix c++ code
/*
config.ini should be next to executable
config.ini format:
	id=<id for the machine>
	cmd=<chromium-browser url?mac=<>&id=<> --flag1 --flag2 .... --flagN>
*/

//change later error from cout to some log file..
char** readConfigFile();
void findLetterLocation(int startPlace, char letterToFind, int fd, int* finalLocation, std::string errorInfo);
std::string findMacAddress();//gets refrence to mac address in main
std::string completeCmd(std::string url, int id, uint64_t macAddress);
uint64_t strMacToIntMac(std::string const& s); //get arr, also mac char should be size 6. read internet
int main(int argc, char* argv[])//parameters are: config.ini
{
	char** args = readConfigFile();
	/*//args test
	for(int i = 0; i < 6; i++)
		std::cout << "args test   --->" << args[i] << "\n";*/
	int pid = fork();
	if(pid == 0)
		if(execvp(args[0], args) == -1)
		{
			std::cout << "could not execute program.";
			std::cout << "args[0]->" << args[0] <<"\n";
			exit(-1);
		};
	//start watch dog
	//std::cout << "WATCH DOG STARTING";
	/*while(1)
	{
		sleep(60)//sleep a minute
		//monitor son aka chromium activity or shit
	}*/
	return 0;
}

char** readConfigFile()
{
	//variable decleration
	std::vector<char*> argsVector;
	char** args = NULL;//to return at end
	int numOfParameters = 0, wordLen = 0;
	char *cmd = NULL, *id = NULL;//to read from configure file
	std::string cmdUnProcessed, sub;
	int idConverted;
	char letter;
	int fd;
	int start = 0, end = 0, size = 0, current = 0;
	//open file
	fd = open("./config.ini", O_RDONLY);//system call to open file in read only mode
	if (fd < 0)
	{
		std::cout << "could not open file\n";
		exit(-1);
	}
	/*ID*/
	findLetterLocation(0, '<', fd, &start, "error finding '<' in ID");//put INDEX of '<' in start
	start++;
	current = start;
	findLetterLocation(current, '>', fd, &end, "error finding '>' in ID");//put INDEX of '>' in end
	size = end - start;
	id = new char[size];
	//read id to id string
	lseek(fd, start, SEEK_SET);//move file pointer to '<' char, AKA start of ID
	if(read(fd, id, size) != size)//read id
	{
		std::cout << "error reading entire id at once\n";
		exit(-1);
	}
	try{idConverted = std::stoi(id);}
	catch(...) {std::cout << "ID OUT OF RANGE OR invalid argument\n";}
	current = end;

	/*cmd*/  //start, end, size - free to use
	//cmd is format < <> <> >
	findLetterLocation(current, '<', fd, &start, "error finding '<' in cmd");//put INDEX of '<' in start
	start++;//move to index of first letter start of command
	current = start;
	findLetterLocation(current, '>', fd, &end, "error finding '>' in cmd");
	current = end + 1;//because end points to index of > and we want to read next >
	findLetterLocation(current, '>', fd, &end, "error finding '>' in cmd");
	current = end + 1;
	findLetterLocation(current, '>', fd, &end, "error finding '>' in cmd");//end of cmd
	current = end;
	size = end - start;
	cmd = new char[size];
	//read url to url string
	lseek(fd, start, SEEK_SET);//move file pointer to '<' char, AKA start of cmd
	if(read(fd, cmd, size) != size)//read cmd
	{
		std::cout << "error reading entire url at once\n";
		exit(-1);
	}//start,end, size - are free to use again
	//make url complete
	cmdUnProcessed = completeCmd(cmd, idConverted, strMacToIntMac(findMacAddress()));
	//chromium-browser url?mac=<>&id=<> --flag1 --flag2 .... --flagN>
	while(!cmdUnProcessed.empty())
	{
		//std::cout << "Current letter is     " << cmdUnProcessed[0] << "\n";
		if(std::isspace(cmdUnProcessed[0]) != 0)//delete all space until non space
		{
			cmdUnProcessed.erase(0,1);
			continue;
		}
		if(!cmdUnProcessed.empty())
			numOfParameters++;
		else break;
		while(std::isspace(cmdUnProcessed[wordLen]) == 0) // if not a space
			wordLen++;
		sub = cmdUnProcessed.substr(0, wordLen);
		argsVector.push_back(strdup(sub.c_str()));
		cmdUnProcessed.erase(0, wordLen);
		wordLen = 0;
	}
	args = new char*[numOfParameters+1];
	for(int i = 0; i < argsVector.size(); i++)
		args[i] = argsVector[i];
	return args;
}

//function to find place of '<' and '>' and update variables sent from main, this func will also get lseek place to start search from. will return -1 if error or something
void findLetterLocation(int startPlace, char letterToFind, int fd, int* finalLocation, std::string errorInfo)
{
	int count = 0;
	char letter;
	lseek(fd, startPlace, SEEK_SET);//move file pointer to startPlace
	if(read(fd, &letter, 1) != 1)
	{
		std::cout << "could not read file letters, " << errorInfo << '\n';
		exit(-1);
	}
	while(letter != letterToFind)
	{
		count++;
		if(read(fd, &letter, 1) != 1)
			{
				//finalLocation++;//will hold the place the char letterToFind is from start of file
				//count++;
				std::cout << "could not read file letters, " << errorInfo << '\n';
				exit(-1);
			}
	}
	*finalLocation = startPlace + count;//this sould point to index of letterToFind
}
std::string findMacAddress()
{
	int fdin, i;
	char tmpLetter;
	std::string mac_address;
	//int pid, fdout, status = 0;
	/*pid = fork();
	if(pid == -1)
	{
		cout << "could not fork\n";
		exit(-1);
	}
	if(pid == 0)
	{
		
		//re route stdout to txt file
		fdout = open("ipconfig.txt",O_TRUNC | O_CREAT | O_RDWR, 0666);//should create the txt file
		fdout = dup2(fdout, 1);
        if(fdout == -1)
       		exit(1);
		//launche ipconfig
		//system("ifconfig -a");
		if(execvp("ifconfig -a", "ifconfig -a") == -1)/////////////////////////////////////////////////a.out in subdir
			exit(1);
	}
	//wait for process to finish
	if(waitpid(pid, &status, 0) != pid)
		exit(1);
	if(status == 1)
		exit(1);*/

	//find mac
	fdin = open("/sys/class/net/eth0/address", O_RDONLY);
	if(fdin < 0)
	{
		std::cout << "error opening /sys/class/net/eth0/address\n";
		exit(-1);
	}
	lseek(fdin, 0, SEEK_SET);//move file pointer to start of file
	for(i=0; i < 17; i++)
	{
		if(read(fdin, &tmpLetter, 1) != 1)
		{
			std::cout << "could not read /sys/class/net/eth0/address to array\n";
			exit(-1);
		}else mac_address += tmpLetter;//lseek(fdin, 1, SEEK_CUR);//jump over the ':'
	}
	//std::cout << mac_address << "\n";
	return mac_address;
}
uint64_t strMacToIntMac(std::string const& s) //get arr, also mac char should be size 6. read internet
{
    unsigned char a[6];
    int last = -1;
    int rc = sscanf(s.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx%n", a + 0, a + 1, a + 2, a + 3, a + 4, a + 5, &last);
    if(rc != 6 || s.size() != last)
        throw std::runtime_error("invalid mac address format " + s);

    return
        uint64_t(a[0]) << 40 |
        uint64_t(a[1]) << 32 |
        uint64_t(a[2]) << 24 |
        uint64_t(a[3]) << 16 |
        uint64_t(a[4]) << 8 |
        uint64_t(a[5]);
}
std::string completeCmd(std::string url, int id, uint64_t macAddress)//url is sent with mac=<>id=<>
{
	//std::cout << "BEFORE COMPLETE URL:\n" << "url is -> "<< url << "\nid is -> " << id << "\nmacAddress is-> " << macAddress << "\n\n";
	std::string str = std::to_string(macAddress);
	std::size_t index = url.find_first_of("=<") + 1;
	std::size_t secIndex = url.find_first_of(">", index) + 1;
	//std::cout << "first index of < is " << index << "\n";
	url.replace(index, secIndex - index, str.data());//second parameter for number of characters to replace
	index = secIndex;//start search for next '<'
	index = url.find_first_of("=<", index) + 1;//now start search from index
	secIndex = url.find_first_of(">", index) +1;
	str = std::to_string(id);
	url.replace(index, secIndex - index, str.data());
	return url;
}
