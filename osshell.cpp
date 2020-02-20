#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <cstring>

 
std::vector<std::string> splitString(std::string text, char d);
std::string getFullPath(std::string cmd, const std::vector<std::string>& os_path_list);
bool fileExists(std::string full_path, bool *executable);

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
            //  If command is `exit` exit loop / quit program
            if(strcmp(input_args[0].c_str(), "exit") == 0)
            {
                //NEED TO RECORD EXIT COMMAND IN HISTORY

                return 0;
            }
            //  If command is `history` print previous N commands
            if(strcmp(input_args[0].c_str, "history") == 0)
            {

            }




        }
    }

    return 0;
}

// Returns vector of strings created by splitting `text` on every occurance of `d`
std::vector<std::string> splitString(std::string text, char d)
{   
    std::vector<std::string> result;
    char text_cstr[text.length()];
    char * c_ptr;
    strcpy(text_cstr, text.c_str());
    //printf("Just after strcpy: %s", text_cstr);
    c_ptr = strtok(text_cstr, &d);
    while(c_ptr != NULL)
    {
        result.push_back(std::string(c_ptr));
        c_ptr = strtok(NULL, &d);
    }

    return result;
}

// Returns a string for the full path of a command if it is found in PATH, otherwise simply return ""
std::string getFullPath(std::string cmd, const std::vector<std::string>& os_path_list)
{
    return "";
}

// Returns whether a file exists or not; should also set *executable to true/false 
// depending on if the user has permission to execute the file
bool fileExists(std::string full_path, bool *executable)
{
    *executable = false;
    return false;
}
