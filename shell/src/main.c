#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <glob.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>
#include <fcntl.h>


/**
 * @brief This is the main function for the shell. It contains the main loop that runs the shell.
 * 
 * @return int 
 */


struct alia{        //structure to store the key value pairs for the alias'
    char *key;      // key has the name of the alias and value has the command
    char *value;
};

struct chain{
    char *command;
    char *operator;
};

int num = 0;                //to keep track of the number of entries in the struct array
struct alia Alias[20];      //to store the structs of all the alias'
int flag_stat = 0;

void cmnd(char *command);
void cmndType2(char *command);
int count(char *input);
void removeLeadingSpaces(char *command);

char* removeQuotations(char *word)         //to remove quotations
{
    if((word[0]=='"' && word[strlen(word)-1]=='"') || (word[0]=='\'' && word[strlen(word)-1]=='\''))
    {
        char *new = (char *)malloc(strlen(word)*sizeof(word));   //assigning memory to the new variable of size 2 lesser than the word as we will be removing two characters.
        int i;
        for(i=1; i<=((int)strlen(word))-2; i++)
        {
            new[i-1] = word[i];
        }
        new[i-1] = '\0';        //to end the string
        return new;
    }
    return word;
}

int count(char *input)      //to count the number of words
{
    int words = 0;
    int i = 0;
    int flag = 0;

    while(input[i]!='\0')       //will work till last character of the string
    {
        if(input[i]=='"')       //this if statement handles the case in which there are quotation marks and treats all the words inside the quotation marks as a single word.
        {
            flag = !flag;       //to toggle the quotations, when the flag is 1 that means that the quotation marks have started and tells the code to count as word.

            i++;

            while(input[i]!='"' && input[i]!= '\0' && input[i]!=' ')
            {
                i++;
            }
            if(flag==1)
            {
                words++;
            }
        }
        else if(input[i]!=' ' && !flag)         //counts the number of words by incrementing the word variable everytime a word is traversed.
        {
            while(input[i]!=' ' && input[i]!='\0')
            {
                i++;
            }
            words++;
        }
        else
        {
            i++;
        }
    }
    return words;
}

int count2(char *input)
{
    int words = 0;
    int i = 0;
    int flag = 0;

    while(input[i]!='\0')       //will work till last character of the string
    {
        if(input[i]=='"')       //this if statement handles the case in which there are quotation marks and treats all the words inside the quotation marks as a single word.
        {
            flag = !flag;       //to toggle the quotations, when the flag is 1 that means that the quotation marks have started and tells the code to count as word.

            i++;

            while(input[i]!='"' && input[i]!= '\0' && input[i]!='|')
            {
                i++;
            }
            if(flag==1)
            {
                words++;
            }
        }
        else if(input[i]!='|' && !flag)         //counts the number of words by incrementing the word variable everytime a word is traversed.
        {
            while(input[i]!='|' && input[i]!='\0')
            {
                i++;
            }
            words++;
        }
        else
        {
            i++;
        }
    }
    return words;
}

char** parser(char *input)      //to parse the strings
{
    int words = count(input);
    char *parse;
    char **parsed = (char **)malloc(strlen(input) * sizeof(char*));     //assigning memory to the double pointer that is words*size of char so as to have enough for all the words

    parse = strtok(input, " ");     //getting the first word from the input, using " " as a divider between words.

    int i=0;
    while(parse!=NULL && i<words)
    {
        if(parse[0]=='"')
        {
            char *string = (char *)malloc((strlen(input)) * sizeof(char*));      //assigning memory to a character array, this variable will store the words inside the quotation marks and treat them as a single word and save it on the parsed array on a single index
            string[0] = '\0';
            strcat(string, parse);      //to concatante the string
            while(parse!=NULL && parse[strlen(parse)-1]!='"')
            {
                parse = strtok(NULL, " ");      //used to get the next word.
                if(parse!=NULL)
                {
                    strcat(string, " ");
                    strcat(string, parse);
                }
            }
            parse = string;
        }
        if(parse[0]=='\'')
        {
            char *string = (char *)malloc((strlen(input)) * sizeof(char));      //assigning memory to a character array, this variable will store the words inside the quotation marks and treat them as a single word and save it on the parsed array on a single index
            string[0] = '\0';
            strcat(string, parse);      //to concatante the string
            while(parse!=NULL && parse[strlen(parse)-1]!='\'')
            {
                parse = strtok(NULL, " ");      //used to get the next word.
                if(parse!=NULL)
                {
                    strcat(string, " ");
                    strcat(string, parse);
                }
            }
            parse = string;
        }
        parsed[i] = strdup(parse); // check for null 
        
        i++;
        parse = strtok(NULL, " ");
    }

    parsed[i] = NULL;

    return parsed;
}

char **parser2(char *input)
{
    char **parsedTwo = (char **)malloc(strlen(input)+1*sizeof(char*));
    
    int i=0;
    int j=0;
    while(input[i]!='\0')
    {
        char* str = (char *)malloc(strlen(input)+1);
        str[0] = '\0';

        while(input[i]!='\0' && input[i]!='|')
        {
            strncat(str, &input[i], 1);
            i++;
            if(input[i]=='"')
            {
                char quote = input[i];
                strncat(str, &input[i], 1);
                i++;
                while(input[i]!='\0' && input[i]!=quote)
                {   
                    strncat(str, &input[i], 1);
                    i++;
                }
            }
        }
        if(input[i]=='|')
        {
            parsedTwo[j] = (char *)malloc(strlen(str)+1);
            strcpy(parsedTwo[j], str);
            i++;
            j++;
        }
        else if(input[i]=='\0')
        {
            parsedTwo[j] = (char *)malloc(strlen(str)+1);
            strcpy(parsedTwo[j], str);
            j++;
        }
    }

    parsedTwo[j] = NULL;

    return parsedTwo;
}

void parser3(struct chain* Commands, char* input, int* comNum) 
{
    int i = 0;
    char* operatorL = NULL;
    LOG_DEBUG("halo\n");
    while (input[i] != '\0') 
    {
        printf("char: %c\n", input[i]);
        char str[200];
        // char* str = (char *)malloc((MAX_STRING_LENGTH + 1) * sizeof(char));
        str[0] = '\0';

        while ((input[i] != '\0') && (input[i] != '&' || input[i + 1] != '&') && (input[i] != '|' || input[i + 1] != '|') && (input[i] != ';')) 
        {
            strncat(str, &input[i], 1);
            i++;

            if (input[i] == '"') {
                char quote = input[i];
                strncat(str, &input[i], 1);
                i++;
                while (input[i] != '\0' && input[i] != quote) {
                    strncat(str, &input[i], 1);
                    i++;
                }
            }
        }

        if ((input[i] != '\0') && ((input[i] == '&' && input[i + 1] == '&') || (input[i] == '|' && input[i + 1] == '|'))) 
        {
            struct chain newCommand;
            newCommand.command = strdup(str);
            newCommand.operator = operatorL;
            if (input[i] == '&' && input[i + 1] == '&') {
                printf("work\n");
                operatorL = "&&";
            } else if (input[i] == '|' && input[i + 1] == '|') {
                operatorL = "||";
            }
            Commands[*comNum] = newCommand;  // inserting the newly created object into the structure array
            (*comNum)++;
            i += 2;
        } 
        else if (input[i] == ';') 
        {
            struct chain newCommand;
            newCommand.command = strdup(str);
            newCommand.operator = operatorL;
            operatorL = ";";
            Commands[*comNum] = newCommand;  // inserting the newly created object into the structure array
            (*comNum)++;
            i++;
        } else if (input[i] == '\0') {
            struct chain newCommand;
            newCommand.command = strdup(str);
            newCommand.operator = operatorL;
            Commands[*comNum] = newCommand;  // inserting the newly created object into the structure array
            (*comNum)++;
        }
        // free(str);

    }
    return;
}

void chaining(char* input) 
{
    int comNum = 0;
    struct chain Commands[25];

    parser3(Commands, input, &comNum);

    printf("NumofCom: %d\n", comNum);

    for (int i = 0; i < comNum; i++) {
        removeLeadingSpaces(Commands[i].command);
        // printf("Command: %s\t", Commands[i].command);
        // printf("Operator: %s\n", Commands[i].operator);
        if(Commands[i].operator!=NULL && flag_stat!=0 && strcmp(Commands[i].operator, "||"))
        {
            cmnd(Commands[i].command);
        }
        else if(Commands[i].operator!=NULL && flag_stat==0 && strcmp(Commands[i].operator, "&&"))
        {
            cmnd(Commands[i].command);
        }
    
        cmnd(Commands[i].command);

    }
    for (int j = 0; j < comNum; ++j) 
    {
        free(Commands[j].command);
    }
}

void processSingleQuotes(char** array) 
{
    int i = 0;

    while (array[i] != NULL) {
        if (strcmp(array[i], "'") == 0 && array[i + 1] != NULL && strcmp(array[i + 1], "'") == 0) {
            
            char* concatenated = malloc(5);
            strcpy(concatenated, "'\x20'");
            
            free(array[i + 1]);
            array[i] = concatenated;

            int j = i + 2;
            while (array[j] != NULL) {
                array[j - 1] = array[j];
                j++;
            }
            array[j - 1] = NULL;
        }

        i++;
    }
}

void removeLeadingSpaces(char *command) 
{
    int length = strlen(command);
    int i = 0;

    while (i < length && command[i] == ' ') {
        i++;
    }

    int j = 0;
    while (i < length) {
        command[j] = command[i];
        i++;
        j++;
    }

    command[j] = '\0';
}

void cmndType2(char *input)
{
    int x = -1;
    char *first = NULL;
    char *second = NULL;
    char *duplicate = (char *)malloc(strlen(input)*sizeof(char*));
    int arr_size = 0;
    arr_size = count(input);
    strcpy(duplicate, input);
    // int counter = 0;
    int y =-1;
    char **arr = parser(duplicate);

    for(int i=0; i<arr_size; i++)
    {
        if(strcmp(arr[i], "|")==0)
        {
            y = i;
            break;
        }
    }


    if(y!=-1 && strcmp(arr[y], "|")==0)       //pipeline
    {
        char *duplicateTwo = (char *)malloc(strlen(input)*sizeof(char*));
        strcpy(duplicateTwo, input);

        char **commands = NULL;
        // int numberOfCommands = count2(duplicateTwo);
        
        commands = parser2(duplicateTwo);

        int numberOfCommands = 0;

        while(commands[numberOfCommands]!=NULL)
        {
            numberOfCommands++;
        }

        int (*fd)[2] = malloc((numberOfCommands-1) * sizeof(int[2]));

        // int fd[numberOfCommands-1][2];
        int child_pid[numberOfCommands];

        for(int i=0; i<numberOfCommands-1; i++)
        {
            if(pipe(fd[i])==-1)
            {
                perror("pipe");
                exit(1);
            }
        }

        for(int i=0; i<numberOfCommands; i++)
        {
        
            child_pid[i] = fork();

            if(child_pid[i] == -1)
            {
                perror("fork");
                exit(1);
            }
            else if(child_pid[i]==0)
            {
                int words = 0;
                char *duple = (char *)malloc(strlen(commands[i])+1);
                strcpy(duple, commands[i]);

                char **arr = parser(commands[i]);

                for(int i=0; arr[i]!=NULL; i++)
                {
                    words++;
                }

                char *arg[400];
                int x = 0;
                
                for(int i=0; i<words; i++)
                {
                    if(strcmp(arr[i],"'")==0 && strcmp(arr[i+1],"'")==0)
                    {
                        processSingleQuotes(arr);
                        words--;
                    }
                }

                for(int i=0; i<words; i++)
                {
                    arg[i] = removeQuotations(arr[i]);
                    x=i;
                }
                arg[x+1] = NULL;


                if(i!=0)
                {   
                    if(dup2(fd[i-1][0], STDIN_FILENO) == -1)                //chaning the file descriptor to read input from the pipe that preceeds them.
                    {
                        perror("dup2");
                        exit(1);
                    }
                    close(fd[i-1][1]);
                    close(fd[i-1][0]);
                }
                if(i!=numberOfCommands-1)
                {
                    close(fd[i][0]);
                    if(dup2(fd[i][1], STDOUT_FILENO) == -1)
                    {
                        perror("dup2");
                        exit(1);
                    }
                    close(fd[i][1]);
                }

                for(int j=0; j<i; j++)
                {
                    close(fd[j][0]);
                    close(fd[j][1]);
                }

                waitpid(child_pid[i], NULL, 0);

                if(strcmp(arg[0], "pwd")==0 || strcmp(arg[0], "cd")==0 || strcmp(arg[0], "alias")==0 || strcmp(arg[0], "unalias")==0 || strcmp(arg[0], "echo")==0 || strcmp(arg[0], "history")==0)
                {
                    cmnd(duple);
                    exit(0);
                }
                for(int i=0; i<words; i++)
                {
                    if(strcmp(arg[i],"<")==0 || strcmp(arg[i],">")==0 || strcmp(arg[i],">>")==0)
                    {
                        cmndType2(duple);
                        exit(0);
                    }
                }
                for(int i=0; i<num; i++)
                {
                    if(strcmp(input, Alias[i].key)==0)
                    {
                        cmnd(removeQuotations(Alias[i].value));     //removing quotations and sending to command function
                        exit(0);
                        return;
                    }
                }
                free(duple);
                execvp(arg[0], arg);
            }
        }

        for(int i=0; i<numberOfCommands-1; i++)
        {
            close(fd[i][0]);
            close(fd[i][1]);
        }

        for(int i=0; i<numberOfCommands; i++)
        {
            waitpid(child_pid[i], NULL, 0);
        }

        for(int i=0; i<numberOfCommands; i++)
        {
            free(commands[i]);
        }
        free(commands);

        return;

    }

    for(int i=0; i<arr_size; i++)
    {
        if(strcmp(arr[i], "<")==0 || strcmp(arr[i], ">")==0 || strcmp(arr[i], ">>")==0)
        {
            x = i;
            break;
        }
    }

    size_t first_len = 1;
    for (int i = 0; i < x; i++) {
        first_len += strlen(arr[i]);
        if(i < x - 1)
            first_len += 1;
    }
    first = (char *)malloc(first_len);

    second = (char *)malloc(strlen(input)-x);

    first[0] = '\0';
    for(int i=0; i<x; i++)
    {
        strcat(first, arr[i]);
        if(i<x-1){
            strcat(first, " ");
        }
    }
    strcat(first, "\0");
    printf("first: %s\n", first);

    second[0] = '\0';

    for(int i=x+1; i<arr_size; i++)
    {
        if(arr[i]!=NULL)
        {
            strcat(second, arr[i]);
            if(i<arr_size-1){
                strcat(second, " ");
            }
        }
    }
    strcat(second, "\0");
    printf("second: %s\n", second);


    if(strcmp(arr[x], "<")==0)        // for input redirection
    {
        int fd = open(second, O_RDONLY);

        if (fd == -1) {
            perror("open");
            exit(1);
        }

        int saved_stdin = dup(STDIN_FILENO);

        if(dup2(fd, STDIN_FILENO) == -1)
        {
            perror("dup2");
            close(fd);
            exit(1);
        }

        cmnd(first);

        if (dup2(saved_stdin, STDIN_FILENO) == -1)      // restoring the original STDIN file descriptor
        {
            perror("dup2");
            close(fd);
            close(saved_stdin);
            exit(1);
        }

        close(fd);
        close(saved_stdin);

        return;
    }
    else if(strcmp(arr[x], ">")==0)       //for output redirection
    {          
        int fd = open(second, O_WRONLY | O_CREAT | O_TRUNC, 0644);

        if (fd == -1) {
            perror("open");
            close(fd);
            exit(1);
        }

        int saved_stdout = dup(STDOUT_FILENO);

        if(dup2(fd, STDOUT_FILENO) == -1)
        {
            perror("dup2");
            close(fd);              //closing duplicated file descriptor
            exit(1);
        }
        // FILE* terminal = fopen("/dev/tty", "w");
        // fprintf(terminal, "ic: %s\n", first);
        // fclose(terminal);
        cmnd(first);

        if(dup2(saved_stdout,STDOUT_FILENO) == -1)
        {
            perror("dup2");
            close(fd);
            close(saved_stdout);
            exit(1);
        }

        close(fd);
        close(saved_stdout);
        
        return;
    }
    else if(strcmp(arr[x], ">>")==0)      //for appending
    {           
        int fd = open(second, O_WRONLY | O_CREAT | O_APPEND, 0644);

        if (fd == -1) {
            perror("open");
            exit(1);
        }

        int saved_stdout = dup(STDOUT_FILENO);

        if (dup2(fd, STDOUT_FILENO) == -1) {
            perror("dup2");
            close(fd);
            exit(1);
        }

        cmnd(first);
        if(dup2(saved_stdout,STDOUT_FILENO) == -1)
        {
            perror("dup2");
            close(fd);
            close(saved_stdout);
            exit(1);
        }

        close(fd);
        close(saved_stdout);

        return;
    }

    free(first);
    free(second);
    free(duplicate);
}

int wildCards(char **input, int *words) {
    glob_t glob_result;
    int x = *words;
    for (int i = 0; i < x; i++) 
    {
        printf("input: %s\n", input[i]);
        int exp = glob(input[i], 0, NULL, &glob_result);

        if (exp != 0) {
            continue;
        }

        char **expanded = NULL;
        size_t expanded_count = 0;

        for (size_t j = 0; j < glob_result.gl_pathc; j++) {
            expanded = realloc(expanded, (expanded_count + 1) * sizeof(char *));
            expanded[expanded_count] = strdup(glob_result.gl_pathv[j]);
            expanded_count++;
        }

        free(input[i]);

        input[i] = strdup(expanded[0]);

        for (size_t j = 1; j < expanded_count; j++) {
            input[*words] = strdup(expanded[j]);
            (*words)++;
        }

        input[*words] = NULL;

        for (size_t j = 0; j < expanded_count; j++) 
        {
            free(expanded[j]);
        }
        // free(expanded);

        globfree(&glob_result);
    }

    return 0;
}

void cmnd(char *input)
{
    for(int i=0; i<num; i++)
    {
        if(strcmp(input, Alias[i].key)==0)
        {
            cmnd(removeQuotations(Alias[i].value));     //removing quotations and sending to command function
            return;
        }
    }


    char *duplicate = (char *)malloc(strlen(input)+2);
    strcpy(duplicate, input);

    char **arr = parser(input);             //parsing the string.

    int words=0;
    for(int i=0; arr[i]!=NULL; i++)
    {
        words++;
    }


    wildCards(arr, &words);

    
    for(int i=0; i<words; i++)
    {
        if(arr[i]!=NULL && (strcmp(arr[i], "&&")==0 || strcmp(arr[i], "||")==0 || strcmp(arr[i], ";")==0))
        {
            chaining(duplicate);
            return;
        }
    }


    for(int i=0; i<words; i++)
    {
        if(strcmp(arr[i], "<")==0 || strcmp(arr[i], ">")==0 || strcmp(arr[i], ">>")==0 || strcmp(arr[i], "|")==0)
        {
            cmndType2(duplicate);
            return;
        }
    }

    
    if(strcmp(arr[0], "pwd")==0)
    {
        char cwd[256];                      //initializing variable
        if (getcwd(cwd, 100) == NULL)       //if getcwd does not get the directory, gives an error, else prints the directory
        {
            perror("There was an error");
        }
        printf("%s\n", cwd);
        flag_stat = 0;
        return;
    }
    else if(strcmp(arr[0], "cd")==0)
    {   
        if(words>2)                             //if words greater than two then gives error
        {
            perror("Too many arguments");
            flag_stat = 1;
            return;
        }
        else if(words==1)                       //if words equal to one that means only 'cd' written and this gives the home directory.
        {
            char *homeDirec = getenv("HOME");
            if (chdir(homeDirec) != 0) 
            {
                perror("chdir");
            }
            flag_stat = 0;
            return;
        }
        else if(words==2)                       //if words greater than two then chdir will navigate to the directory specified.
        {
            char *word = arr[1];
            if(chdir(word)!=0)
            {
                perror("no such directory");
            }
            flag_stat = 0;
            return;
        }
    }
    else if(strcmp(arr[0], "alias")==0)
    {
        if(words==1)
        {
            for(int i=0; i<num; i++)
            {
                printf("%s='%s'\n", Alias[i].key, removeQuotations(Alias[i].value));
            }
        }
        else if(words==2)
        {
            for(int i=0; i<num; i++)
            {
                if(strcmp(arr[1],Alias[i].key)==0)
                {
                    printf("%s='%s'\n", Alias[i].key, removeQuotations(Alias[i].value));
                }
            }
        }
        else if(words==3)
        {
            struct alia newAlias;
            newAlias.key = malloc(strlen(arr[1])+1);
            strcpy(newAlias.key, arr[1]);
            newAlias.value = malloc(strlen(arr[2])+1);
            strcpy(newAlias.value, arr[2]);
            Alias[num] = newAlias;              //inserting the newly created object into the structure array
            num++;
        }
    }
    else if(strcmp(arr[0], "unalias")==0)
    {
        int x = -1;
        for(int i=0; i<num; i++)
        {
            if(strcmp(arr[1], Alias[i].key)==0)
            {
                x = i;
                break;
            }
        }

        if(x >= 0)
        {
            free(Alias[x].key);                     //freeing the key and value values at index x where the unaliasing is taking place.
            free(Alias[x].value);

            for (int i=x; i<num-1; i++) {         //moving the objects infront of the deleted object one step backwards.
                Alias[i] = Alias[i + 1];
            }

            num--;
        }
    }
    else if(strcmp(arr[0], "echo")==0)
    {
        int flag = 0;
        char *word = NULL;
        int j = 0;
        word = (char *)malloc(strlen(duplicate) + 1);

        for(int i=5; i<(int)strlen(duplicate); i++)
        {

            if(i<(int)strlen(duplicate)){
            if(duplicate[i]=='"')
            {
                flag = !flag;
                i++;
                while(flag==1)
                {
                    if(duplicate[i]=='"'){
                        flag=!flag;
                        word[j] = ' ';

                        j++;
                        i++;
                        break;
                    }
                    word[j] = duplicate[i];

                    j++;
                    i++;
                }
            }
            else if(duplicate[i]!=' ')
            {
                while(i< (int)strlen(duplicate) && duplicate[i]!=' ')
                {
                    word[j] = duplicate[i];
                    j++;
                    i++;
                }
                word[j] = ' ';
                j++;

            }
            else if(duplicate[i]==' ')
            {
                i++;
                if (i < (int)strlen(duplicate) && duplicate[i] != ' ') {
                    word[j] = ' ';
                    j++;
                }
            }
        }
        }
        word[j] = '\0';
        printf("%s\n",word);
        free(word);
        flag_stat = 0;
        return;
    }
    else if(strcmp(arr[0], "history")==0)
    {
        HIST_ENTRY **historyList = history_list();
        if(words==2)
        {
            if(atoi(arr[1])-1>=0 && atoi(arr[1])-1<history_length)
            {
                cmnd(historyList[atoi(arr[1])-1]->line);
            }
            flag_stat = 0;
        }
        else if(words==1)
        {
            if(historyList){
                for(int i=0; historyList[i]!=NULL; i++)
                {
                    printf("%d %s", i+history_base, historyList[i]->line);
                    printf("\n");
                }
            }
            flag_stat = 0;
        }
    }
    else
    {
        char *arg[50];
        int x = 0;
        
        for(int i=0; i<words; i++)
        {
            if(strcmp(arr[i],"'")==0 && strcmp(arr[i+1],"'")==0)
            {
                processSingleQuotes(arr);
                words--;
            }
        }

        for(int i=0; i<words; i++)
        {
            arg[i] = removeQuotations(arr[i]);
            x=i;
        }
        arg[x+1] = NULL;


        // for(int i=0; i<words; i++)
        // {
        //    printf("omg: %d%s\n", i, arg[i]);
        // }

        wait(NULL);

        int rc = fork();

        if(rc<0)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if(rc==0)
        {

            if(execvp(arg[0], arg)==-1)
            {
                perror("execvp");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            flag_stat = 0;
            wait(NULL);
        }
    free(duplicate);
    }
    free(arr);
}

int main(int argc, char** argv)
{
    rl_bind_key('\t', rl_complete);

    using_history();

    char *filename = argv[1];

    if(argc==2)
    {
        FILE *file;             //file pointer that points at the start of the file
        char *line = NULL;
        size_t line_size = 0;
        
        file = fopen(filename,"r");

        while(getline(&line, &line_size, file)!=-1)     //reads the file line by line and increments the file pointer
        {
            if(!feof(file))                             //removing the "\n" from the lines, except for the last line.
            {line[strlen(line) - 1] = '\0';}
            if(strcmp(line, "exit")==0){                //if the entry says exit then breaks the loop
                exit(0);
            }
            add_history(line);
            cmnd(line);
        }

        free(line);

        fclose(file);
    }
    else
    {
        while(1)
        {
            char* input = readline("prompt> "); // possible memory leak

            if (strlen(input) == 0) 
            {
                free(input);
                continue;
            }
            
            if(strcmp(input, "exit")==0){
                exit(0);
            }

            if(!input)
            {
                break;
            }

            add_history(input);

            cmnd(input);

            free(input);
        }
    }
    
    return 0;
}
