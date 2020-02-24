#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>


std::vector<std::string> splitString(std::string text, char d);
std::string getFullPath(std::string cmd, const std::vector<std::string>& os_path_list);
bool fileExists(std::string full_path, bool *executable);
char*const* buildExecArgs(std::vector<std::string> input_args);

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

    while(1)
    {
        //  Print prompt for user input: "osshell> " (no newline)
        std::cout << "osshell>";
        //  Get user input for next command
        std::getline(std::cin, input);
        input_args  = splitString(input, ' ');
        if(input_args.size() > 0)
        {
            std::string cmd = input_args[0];
            //  If command is `exit` exit loop / quit program
            if(strcmp(cmd.c_str(), "exit") == 0)
            {
                //NEED TO RECORD EXIT COMMAND IN HISTORY

                return 0;
            }
            //  If command is `history` print previous N commands
            if(strcmp(cmd.c_str(), "history") == 0)
            {
                
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


                    //execute

                }
                else
                {//   If no, print error statement: "<command_name>: Error running command" (do include newline)
                    std::cout << cmd + ": Error running command\n";
                }
                

            }
            




        }
    }

    return 0;
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
            //return os_path_list[i] + "/";
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
