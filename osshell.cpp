#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <fstream>


std::vector<std::string> splitString(std::string text, char d);
std::string getFullPath(std::string cmd, const std::vector<std::string>& os_path_list);
bool fileExists(std::string full_path, bool *executable);
char*const* buildExecArgs(std::vector<std::string> input_args);
void recordHistory(std::string input, const char* path);


int main (int argc, char **argv)
{
    std::string input;
    char* os_path = getenv("PATH");
    std::vector<std::string> os_path_list = splitString(os_path, ':');

    std::cout << "Welcome to OSShell! Please enter your commands ('exit' to quit)." << std::endl;

    // Repeat:
    //  Print prompt for user input: "osshell> " (no newline)
    //  Get user input for next command
    //  If command is `exit` exit loop / quit program
    //  If command is `history` print previous N commands
    //  For all other commands, check if an executable by that name is in one of the PATH directories
    //   If yes, execute it
    //   If no, print error statement: "<command_name>: Error running command" (do include newline)

    std::string exit = "exit";
    std::vector<std::string> input_args;
    std::vector<std::string> quote_split;
    std::vector<std::string> space_split;

    while(1)
    {
        //  Print prompt for user input: "osshell> " (no newline)
        std::cout << "osshell>";
        //  Get user input for next command
        std::getline(std::cin, input);

        //split on quotes
        quote_split = splitString(input, '\"');
       
        //split on spaces for segments that are not between quotes
        for(int i = 0; i < quote_split.size(); i++)
        {
            if(i % 2 == 0)
            {
                space_split = splitString(quote_split[i], ' ');
                for(int j=0; j < space_split.size(); j++)
                {
                    input_args.push_back(space_split[j]);
                }
            }
            else
            {
                input_args.push_back(quote_split[i]);
            }
        }
        
        if(input_args.size() > 0)
        {
            //record input into history
            const char * path = "/home/keith/Desktop/history";
            std::string cmd = input_args[0];
            int clear = 0;

            //  If command is `exit` exit loop / quit program
            if(strcmp(cmd.c_str(), "exit") == 0)
            {
                recordHistory(std::string(input), path);
                return 0;
            }
            //  If command is `history` print previous N commands
            if(strcmp(cmd.c_str(), "history") == 0)
            {
                if(input_args.size() > 1)
                {
                    if(strcmp(input_args[1].c_str(), "clear") == 0)
                    {
                        FILE * historyFile;
                        if((historyFile = fopen(path, "w")) != NULL)
                        {
                            fclose(historyFile);
                        }
                        //history clear command will not be logged
                        clear = 1;
                    }
                    else
                    {   //dump last n commands
                        std::ifstream in(path);
                        std::string line;
                        std::vector<std::string> historyLines;
                        int n;
                        n = atoi(input_args[1].c_str());
                        if(n <= 0)
                        {
                            std::cout << "Error: history expects an integer > 0 (or 'clear')";
                        }
                        else
                        {
                            //read history
                            while(std::getline(in, line))
                            {
                                historyLines.push_back(line);
                            }
                            if(historyLines.size() > 0)
                            {
                                int start = historyLines.size() - n;
                                for(int i = std::max(start, 0); i < historyLines.size(); i++)
                                {
                                    printf("%d: %s\n", i+1, historyLines[i].c_str());
                                }
                            }

                        }
                    }
                }
                else
                {
                    //dump all history 
                    std::ifstream in(path);
                    std::string line;
                    std::vector<std::string> historyLines;
                    while(std::getline(in, line))
                    {
                        historyLines.push_back(line);
                    }
                    if(historyLines.size() > 0)
                    {
                        for(int i = 0; i < historyLines.size(); i++)
                        {
                            printf("%d: %s\n", i+1, historyLines[i].c_str());
                        }
                    }
                }
            }
            else
            {
                //For all other commands, check if an executable by that name is in one of the PATH directories
                std::string path = getFullPath(cmd, os_path_list);
                bool executable = false;
                char * const * exec_args = buildExecArgs(input_args);

                //   If yes, execute it
                if(fileExists(path, &executable) && executable)
                {
                    //fork
                    pid_t c_pid;
                    int status;
                    c_pid = fork();
                    //printf("pid: %d\n", c_pid);

                    if( c_pid == 0 ){
                        //child
                        char * const * exec_args = buildExecArgs(input_args);
                        int success = execv(path.c_str(), exec_args);
                        delete(exec_args);
                        return 0;
                    }
                    else if(c_pid > 0)
                    {//parent
                        wait(&status);
                        //printf("%d", WIFEXITED(status));
                    }
                    else{
                    //failure
                     printf("Failed to fork");
                    _exit(2);//hard exit
                    }

                }
                else
                {//   If no, print error statement: "<command_name>: Error running command" (do include newline)
                    std::cout << cmd + ": Error running command\n";
                }
            }
            if(clear != 1)
            {
                recordHistory(std::string(input), path);
            }

            input_args.clear();
        }
    }

    return 0;
}







void recordHistory(std::string input, const char* path)
{
    //read in history
    std::ifstream in;
    std::string line;
    std::vector<std::string> historyLines;

    //read in the file
    in.open(path);
    while(std::getline(in, line))
    {
        historyLines.push_back(line);
    }
    in.close();

    //add new entry
    historyLines.push_back(input);

    //check number of lines
    if(historyLines.size() > 128)
    {   //erase first line
        historyLines.erase(historyLines.begin());
    }

    std::ofstream out;
    //rewrite to file
    out.open(path, std::ios_base::trunc);
    out.close();
    out.open(path, std::ios_base::app);
    for(int i = 0; i < historyLines.size(); i++)
    {
        out << historyLines[i] << "\n";
    }
    out.close();
}



char*const* buildExecArgs(std::vector<std::string> input_args)
{
    char ** exec_args = new char * [input_args.size() + 1];
    for(int i=0; i < input_args.size(); i++)
    {
        exec_args[i] = new char[input_args[i].size()];
        strcpy(exec_args[i], input_args[i].c_str());
    }
    exec_args[input_args.size()] = NULL;

    return exec_args;
}

// Returns vector of strings created by splitting `text` on every occurance of `d`
std::vector<std::string> splitString(std::string text, char d)
{   
    std::vector<std::string> result;
    char text_cstr[text.length()];
    char * c_ptr;
    strcpy(text_cstr, text.c_str());
    c_ptr = strtok(text_cstr, &d);
          //  printf("%s\n", c_ptr);

    while(c_ptr != NULL)
    {
        result.push_back(std::string(c_ptr));
        c_ptr = strtok(NULL, &d);
       // printf("%s\n", c_ptr);
    }
    //std::cout << result.back();
    return result;
}

// Returns a string for the full path of a command if it is found in PATH, otherwise simply return ""
std::string getFullPath(std::string cmd, const std::vector<std::string>& os_path_list)
{
    std::string tryPath;
    bool executable = false;
    for(int i=0; i < os_path_list.size(); i++)
    {
        tryPath = os_path_list[i] + "/" + cmd;
        if(fileExists(tryPath, &executable))
        {
            return tryPath;
        }
    }
    return "";
}

// Returns whether a file exists or not; should also set *executable to true/false 
// depending on if the user has permission to execute the file
bool fileExists(std::string full_path, bool *executable)// <-- getFullPath should call this to check for each path
{
    *executable = false;
    bool result = false;
    if( access(full_path.c_str(), F_OK) == 0){result = true;}
    if( access(full_path.c_str(), X_OK) == 0){*executable = true;}
    return result;
}
