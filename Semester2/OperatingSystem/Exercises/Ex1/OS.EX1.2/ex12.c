/*********************************
 * Student Name: Shachar sirotkin
 * Student ID: 208994962
 * Course Exercise Group: 06
 * Excecise name: ex12.c
 *********************************/
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>

//enum indicates the result of the search of the c file
enum FIND_C_FILE_GRADE_REASON {
    NO_C_FILE = 1, MULTIPLE_DIRECTORIES = 2, WRONG_DIRECTORY = 3,
    GOOD_FILE_FOUND = 4
};
//enum indicates the result of the compile and run level,
// when running the c file that was found in the search level
enum COMPILE_AND_RUN_GRADE_REASON {
    COMPILE_NONE = 0, COMPILATION_ERROR = 5, TIMEOUT = 6, GOOD_RUN = 7
};
//enum indicates the result of the compare level,
// the comparing between the correct output and the c file running result
enum COMPARE_GRADE_REASON {
    COMPARE_NONE = 0, GREAT_JOB = 8, SIMILLAR_OUTPUT = 9, BAD_OUTPUT = 10
};

//struct that represent the result of searching for the c file.
// contain student name, the path to his c file,
// the depth where the c file was found
// and the explanation of the search result
struct CPathReport {
    char *studentName;
    char *cPath;
    enum FIND_C_FILE_GRADE_REASON findCGradeReason;
    int depth;
};

typedef struct CPathReport CPathReport;

/***********************************
 * func name: CalculateGrade
 * input: findGradeReason - a FIND_C_FILE_GRADE_REASON instance.
 *              indicates the result of the search level.
 *        compareGradeReason - a COMPILE_AND_RUN_GRADE_REASON instance.
 *              indicates the result of the compile and run level.
 *        compileRunGradeReason - a COMPARE_GRADE_REASON instance.
 *              indicates the result of the compare level.
 *        depth - the depth where the c file was found.
 * output: the final grade of the student regarding every reason and the depth.
 * func operration: calculates the final grade of the student
 *                  regarding every reason and the depth.
 ****************************/
int CalculateGrade(enum FIND_C_FILE_GRADE_REASON findGradeReason,
                   enum COMPARE_GRADE_REASON compareGradeReason,
                   enum COMPILE_AND_RUN_GRADE_REASON compileRunGradeReason,
                   int depth);

/*******
 * func name: CheckError
 * input: x - a number represents aresult of system call.
 *        s - a string for perror if the system call failed
 * output: no output
 * func operation: checking whether the x num meant
 *                 that the sysytem call failed.
 *                 write perror and finish the program if it failed.
 */
void CheckError(int x, char *s);

/*******
 * func name: IsDir
 * input: innerDirPath - a path to innerDirent.
 * output: 1 if the dirent is directory. else, return 0.
 * func operation: checking whether the dirent is directory.
 */
int IsDir(char *innerDirPath);

/*******
 * func name: FindCFile
 * input: currentPath - the path to the current directory.
 *        cFilePathReport - a cFileReport struct instance.
 *        depth - the current depth relative to the main directory.
 *                should be 0 in the first level of the search.
 * output: no output.
 * func operation: Search for c file hierarchicaly and recursively
 *                 starting from the first currentPath.
 *                 If there is no c file in the heirarchy or if in one level
 *                 there are more than one directory, the cFilePathReport
 *                 will contain NULL as cPath.
 *                 If c file was found the cPath will be the path to it.
 *                 If there is a c file but not in the first level,
 *                 the FIND_C_FILE_GRADE_REASON will be marked as
 *                 WRONG_DIRECTORY.
 */
void FindCFile(char *currentPath, CPathReport *cFilePathReport, int depth);

/*******
 * func name: CompileAndRunCFile
 * input: pathToCFile - path to c file that was found in the search level.
 *        pathToInput - path to the file that contain the input for
 *                      the process of the c file.
 * output: COMPILE_AND_RUN_GRADE_REASON instance.
 *         Indicates the result of the compile and run level.
 * func operation: compile and run the c file and return a
 *                 COMPILE_AND_RUN_GRADE_REASON instance that indicates the
 *                 logical result.
 */
enum COMPILE_AND_RUN_GRADE_REASON CompileAndRunCFile
        (char *pathToCFile, char *pathToInput);

/****
 * func name: RunCompareOutputs
 * input: correctOutputPath - path to the correct output file.
 *        output - the output from the c file running.
 * output: COMPARE_GRADE_REASON instance.
 *         indicates for the result of the comparing.
 * func operation: run external text comparator process
 *                 and return COMPARE_GRADE_REASON instance
 *                 according to its result.
 */
enum COMPARE_GRADE_REASON
RunCompareOutputs(char *correctOutputPath, char *output);

/****
 * func name: CreateResultString
 * input: resultString - a pointer to chars that will contain the string
 *                      describing the result of the search, compile,
 *                      run and compare levels
 *        studentName - the name of the current student
 *        FindGradeReason - a FIND_C_FILE_GRADE_REASON instance.
 *        compareGradeReason - a COMPARE_GRADE_REASON instance.
 *        runGradeReason - a COMPILE_AND_RUN_GRADE_REASON instance.
 *        depth - the depth on the heirarchy where the c file was found.
 * output: no output.
 * func operation: set resultString to a string describing the result
 *                 of the search, compile, run and compare levels
 *                 according to the Reasons enums.
 */
void CreateResultString(char *resultString, char *studentName,
                        enum FIND_C_FILE_GRADE_REASON FindGradeReason,
                        enum COMPARE_GRADE_REASON compareGradeReason,
                        enum COMPILE_AND_RUN_GRADE_REASON runGradeReason,
                        int depth);

/****
 * func name: CreateInnerDirentPath
 * input: path - the path to the current directory.
 *        innerDirent - an inner dirent in the directory.
 * output: a path to the inner dirent.
 * func operation: create a full path to the inner dirent.
 */
char *CreateInnerDirentPath(char *path, struct dirent *innerDirent);

/***
 * func name: GetGradeReason.
 * input: findReason - a FIND_C_FILE_GRADE_REASON instance.
 *        compareReason - a COMPARE_GRADE_REASON instance.
 *        runGradeReason - a COMPILE_AND_RUN_GRADE_REASON instance.
 * output: the final grade according to the Reasons enums.
 * func operation: calculates the final grade according to the Reasons enums.
 */
char *GetGradeReason(enum FIND_C_FILE_GRADE_REASON findReason,
                     enum COMPARE_GRADE_REASON compareReason,
                     enum COMPILE_AND_RUN_GRADE_REASON runGradeReason);

/******
 * func name: main
 * input: argc - the num of args.
 *        argv - the args to the program.
 * output: 0
 * func operation: Open the configuration file and extract the info.
 *                 Open each inner dirent in the main directory,
 *                 search for c file in it, compile and run the c file,
 *                 compare its output to the correct output and write the
 *                 result to csv file.
 */
int main(int argc, char *argv[]) {
    if (argc != 2) {
        return -1;
    }
    //buffer for reading the config file
    char configBuffer[480];
    //open the config file
    int fd = open(argv[1], O_RDONLY);
    //checking whether the open system call failed
    CheckError(fd, "opening config file failed");
    //read the config file into buffer
    int readResult = (int) read(fd, configBuffer, 480);
    //checking whether the read sysytem call failed
    CheckError(readResult, "reading config file failed");
    //closing the config file while checking for error in the sysytem call
    CheckError(close(fd), "closing config file failed");
    //getting the 3 config line to separate buffers
    char *mainDirPath = strtok(configBuffer, "\n");
    char *inputFilePath = strtok(NULL, "\n");
    char *outputFilePath = strtok(NULL, "\n");
    int fdOutput;
    int fdBackup;
    int resultFD;
    //open the results file, trunc it if it's exist, create it if it's not
    resultFD = open("./results.csv",
                    O_WRONLY | O_APPEND | O_CREAT | O_TRUNC, 0666);
    //checking for error in openning the results file
    CheckError(resultFD, "open result file failed");
    //getting the main dir from the main dir config line
    DIR *mainDir = opendir(mainDirPath);
    //checking for error in openning the main dir
    if (mainDir == NULL) {
        perror("open main dir failed");
        exit(-1);
    }
    struct dirent *dirent;
    //allocates memory for cFileReport instance
    struct CPathReport *cPathReport = malloc(sizeof(cPathReport));
    //set COMPARE_GRADE_REASON and COMPILE_AND_RUN_GRADE_REASON
    // instances to default value
    enum COMPARE_GRADE_REASON compareGradeReason = COMPARE_NONE;
    enum COMPILE_AND_RUN_GRADE_REASON compileRunGradeReason = COMPILE_NONE;
    //while there is more dirents in the dir, search for c file inside them
    // (if they are directories, of course), compile and run it with the input
    // from the input config line, and compare the result of the running with
    // the output from the output config libe
    while ((dirent = readdir(mainDir)) != NULL) {
        //open file for the output from the running of the c file
        fdOutput = open("./output.txt", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR |
                                                                    S_IWUSR);
        //checking for error in the output file openning
        CheckError(fdOutput, "open output file failed");
        //create backup of the fd by dup, checking for error in the operation
        CheckError(fdBackup = dup(fdOutput), "dup output failed");
        //direct the output from stdout to the output file
        CheckError(dup2(fdOutput, 1), "dup2 failed");
        //create a path to the dirent
        char *innerDirPath = CreateInnerDirentPath(mainDirPath, dirent);
        //if the dirent is . or .. or doesn't a dir skip it for avoiding
        // loops in the grading and another files
        if (strcmp(dirent->d_name, ".") == 0 ||
            strcmp(dirent->d_name, "..") == 0 ||
            !IsDir(innerDirPath)) {
            continue;
        }
        //set the cPathReport's studentName to the current dir name
        cPathReport->studentName = dirent->d_name;
        //find the path to c file if it exists or getting NULL if it's not
        FindCFile(innerDirPath, cPathReport, 0);
        //if the searching succeeded, try compile and run it
        if (cPathReport->findCGradeReason == GOOD_FILE_FOUND ||
            cPathReport->findCGradeReason == WRONG_DIRECTORY) {
            compileRunGradeReason = CompileAndRunCFile(cPathReport->cPath,
                                                       inputFilePath);
            //if the compile and run succeeded, compare the output with
            // the correct one
            if (compileRunGradeReason == GOOD_RUN) {
                compareGradeReason = RunCompareOutputs(outputFilePath,
                                                       "./output.txt");
            }
        }
        //create a string contain the result of the student's c file
        char resultString[100];
        CreateResultString(resultString, cPathReport->studentName,
                           cPathReport->findCGradeReason,
                           compareGradeReason, compileRunGradeReason,
                           cPathReport->depth);
        //writing the result to results.csv while checking for errors
        // in the system call
        CheckError((int) write(resultFD, resultString, strlen(resultString)),
                   "writing to result.csv failed");
        //closes the output file ehile checking for error
        CheckError(close(fdBackup), "closing output fd failed");
        free(innerDirPath);
        //reset the compile and run and compile enums
        compareGradeReason = COMPARE_NONE;
        compileRunGradeReason = COMPILE_NONE;
    }
    //close the main dir while checking for errors
    CheckError(closedir(mainDir), "closing dir failed");
    //close the reult file while checking for errors
    CheckError(close(resultFD), "closing result fd failed");
    //unlinking the output and a.out files while checking for errors
    CheckError(unlink("./output.txt"), "unlinking a.out failed");
    CheckError(unlink("./a.out"), "unlinking a.out failed");
    return 0;
}

/****
 * func name: CreateInnerDirentPath
 * input: path - the path to the current directory.
 *        innerDirent - an inner dirent in the directory.
 * output: a path to the inner dirent.
 * func operation: create a full path to the inner dirent.
 */
char *CreateInnerDirentPath(char *path, struct dirent *innerDirent) {
    //allocates memory for the inner dir path
    char *innerDirPath = (char *) malloc(strlen(path)
                                         + strlen(innerDirent->d_name) + 2);
    //copying the current path
    strcpy(innerDirPath, path);
    //adding the next level to the current inner dirent
    strcat(innerDirPath, "/");
    strcat(innerDirPath, innerDirent->d_name);
    strcat(innerDirPath, "\0");
    return innerDirPath;
}

/****
 * func name: CreateResultString
 * input: resultString - a pointer to chars that will contain the string
 *                      describing the result of the search, compile,
 *                      run and compare levels
 *        studentName - the name of the current student
 *        FindGradeReason - a FIND_C_FILE_GRADE_REASON instance.
 *        compareGradeReason - a COMPARE_GRADE_REASON instance.
 *        runGradeReason - a COMPILE_AND_RUN_GRADE_REASON instance.
 *        depth - the depth on the heirarchy where the c file was found.
 * output: no output.
 * func operation: set resultString to a string describing the result
 *                 of the search, compile, run and compare levels
 *                 according to the Reasons enums.
 */
void CreateResultString(char *resultString, char *studentName,
                        enum FIND_C_FILE_GRADE_REASON findGradeReason,
                        enum COMPARE_GRADE_REASON compareGradeReason,
                        enum COMPILE_AND_RUN_GRADE_REASON compileRunGradeReason,
                        int depth) {
    int grade = 0;
    //allocates memory for the grade
    char *gradeString = malloc(3 * sizeof(char));
    //copying the student name
    strcpy(resultString, studentName);
    strcat(resultString, ",");
    //calculates the grade according to the Reasons enums
    grade = CalculateGrade(findGradeReason, compareGradeReason,
                           compileRunGradeReason, depth);
    //convert the grade to char * instance
    sprintf(gradeString, "%d", grade);
    //adding the grade to the result string
    strcat(resultString, gradeString);
    strcat(resultString, ",");
    //create the grade explanation according to the Reasons enums
    char *gradeReason = GetGradeReason(findGradeReason, compareGradeReason,
                                       compileRunGradeReason);
    //adding the grade explanation to the result string
    strcat(resultString, gradeReason);
    free(gradeReason);
    free(gradeString);
}

/***
 * func name: GetGradeReason.
 * input: findReason - a FIND_C_FILE_GRADE_REASON instance.
 *        compareReason - a COMPARE_GRADE_REASON instance.
 *        runGradeReason - a COMPILE_AND_RUN_GRADE_REASON instance.
 * output: the final grade according to the Reasons enums.
 * func operation: calculates the final grade according to the Reasons enums.
 */
char *GetGradeReason(enum FIND_C_FILE_GRADE_REASON findReason,
                     enum COMPARE_GRADE_REASON compareReason,
                     enum COMPILE_AND_RUN_GRADE_REASON compileRunGradeReason) {
    //allocates memory for the grade explanation
    char *gradeReason = (char *) malloc(60 * sizeof(char));
    strcpy(gradeReason, "");
    //array converts enums to explanation strings
    char *enumStrings[] = {"", "NO_C_FILE", "MULTIPLE_DIRECTORIES",
                           "WRONG_DIRECTORY", "",
                           "COMPILATION_ERROR", "TIMEOUT", "",
                           "GREAT_JOB", "SIMILLAR_OUTPUT", "BAD_OUTPUT"};
    //find the right explanation according to the reasons enums
    if (findReason == NO_C_FILE || findReason == MULTIPLE_DIRECTORIES) {
        strcat(gradeReason, enumStrings[findReason]);
    } else {
        if (compileRunGradeReason == TIMEOUT ||
            compileRunGradeReason == COMPILATION_ERROR) {
            strcat(gradeReason, enumStrings[compileRunGradeReason]);
        } else {
            //WRONG_DIRECTORY can be added to the compare reasons
            if (findReason == WRONG_DIRECTORY) {
                strcat(gradeReason, enumStrings[findReason]);
                strcat(gradeReason, ",");
            }
            strcat(gradeReason, enumStrings[compareReason]);
        }
    }
    strcat(gradeReason, "\n\0");
    return gradeReason;
}

/***********************************
 * func name: CalculateGrade
 * input: findGradeReason - a FIND_C_FILE_GRADE_REASON instance.
 *              indicates the result of the search level.
 *        compareGradeReason - a COMPILE_AND_RUN_GRADE_REASON instance.
 *              indicates the result of the compile and run level.
 *        compileRunGradeReason - a COMPARE_GRADE_REASON instance.
 *              indicates the result of the compare level.
 *        depth - the depth where the c file was found.
 * output: the final grade of the student regarding every reason and the depth.
 * func operration: calculates the final grade of the student
 *                  regarding every reason and the depth.
 ****************************/
int CalculateGrade(enum FIND_C_FILE_GRADE_REASON findGradeReason,
                   enum COMPARE_GRADE_REASON compareGradeReason,
                   enum COMPILE_AND_RUN_GRADE_REASON compileRunGradeReason,
                   int depth) {
    int grade = 0;
    //calculates the basic grade according to the Reasons enums
    if (findGradeReason == MULTIPLE_DIRECTORIES) {
        grade = 0;
    } else {
        if (compileRunGradeReason == COMPILATION_ERROR ||
            compileRunGradeReason == TIMEOUT) {
            grade = 0;
        } else {
            if (compareGradeReason == SIMILLAR_OUTPUT) {
                grade = 70;
            }
            if (compareGradeReason == BAD_OUTPUT) {
                grade = 0;
            }
            if (compareGradeReason == GREAT_JOB) {
                grade = 100;
            }
        }
    }
    //WRONG_DIRECTORY decreases the grade according the depth where
    // the c file was found
    if (grade > 0 && findGradeReason == WRONG_DIRECTORY) {
        grade -= 10 * depth;
    }
    //grade can not be less then 0
    if (grade < 0) {
        grade = 0;
    }
    return grade;
}

/****
 * func name: RunCompareOutputs
 * input: correctOutputPath - path to the correct output file.
 *        output - the output from the c file running.
 * output: COMPARE_GRADE_REASON instance.
 *         indicates for the result of the comparing.
 * func operation: run external text comparator process
 *                 and return COMPARE_GRADE_REASON instance
 *                 according to its result.
 */
enum COMPARE_GRADE_REASON RunCompareOutputs(char *correctOutputPath,
                                            char *output) {
    int pid;
    int status;
    CheckError(pid = fork(), "forking failed");
    if (pid == 0) {
        //comparing the output and the correct output by using the
        // external process "comp.out"
        char *args[] = {"./comp.out", output, correctOutputPath, NULL};
        //checking for error in the execution
        CheckError(execvp("./comp.out", args), "execute comp.out failed");
    } else {
        //waiting for the end of the comparing process
        // while checking for errors
        CheckError(waitpid(pid, &status, 0), "waiting for comp.out failed");
        if (WIFEXITED(status)) {
            //getting the return value of comp.out
            int result = WEXITSTATUS(status);
            //return the comparing logical reason according to this result
            switch (result) {
                case 1:
                    return GREAT_JOB;
                case 2:
                    return SIMILLAR_OUTPUT;
                case 3:
                    return BAD_OUTPUT;
                default:
                    return COMPARE_NONE;
            }
        }
    }
    return COMPARE_NONE;
}

/*******
 * func name: FindCFile
 * input: currentPath - the path to the current directory.
 *        cFilePathReport - a cFileReport struct instance.
 *        depth - the current depth relative to the main directory.
 *                should be 0 in the first level of the search.
 * output: no output.
 * func operation: Search for c file hierarchicaly and recursively
 *                 starting from the first currentPath.
 *                 If there is no c file in the heirarchy or if in one level
 *                 there are more than one directory, the cFilePathReport
 *                 will contain NULL as cPath.
 *                 If c file was found the cPath will be the path to it.
 *                 If there is a c file but not in the first level,
 *                 the FIND_C_FILE_GRADE_REASON will be marked as
 *                 WRONG_DIRECTORY.
 */
void FindCFile(char *currentPath, CPathReport *cFilePathReport, int depth) {
    char nextDirPath[161];
    nextDirPath[0] = '\0';
    struct dirent *tempDirent;
    char *innerDirentPath = "";
    char *cFilePath = NULL;
    char *extension;
    int multipleDirs = 0;
    DIR *currentDir;
    //if open the dir failed exit the process
    if ((currentDir = opendir(currentPath)) == NULL) {
        perror("open dir failed");
        exit(-1);
    }
    //while there is another dirent in the dir, check whether it's dir
    // or c file.
    //when the first dir is found save its path, when more then one are found
    // mark it
    // when c file is found save it and stop the searching
    while ((tempDirent = readdir(currentDir)) != NULL) {
        innerDirentPath = CreateInnerDirentPath(currentPath, tempDirent);
        if (IsDir(innerDirentPath)) {
            //if the dir is . or .. skip on it
            if (!(strcmp(tempDirent->d_name, ".")) ||
                !(strcmp(tempDirent->d_name, ".."))) {
                continue;
            }
            //if it's not the first dir, mark it
            if (strcmp(nextDirPath, "")) {
                multipleDirs = 1;
            }
            //saving the next dir path that was found
            strcpy(nextDirPath, innerDirentPath);
            //if it's not dir, its a file
        } else {
            //getting the extension of the file
            extension = strrchr(tempDirent->d_name, '.');
            //if the file is c file save it and stop the searching
            if (strcmp(extension, ".c") == 0) {
                cFilePath = innerDirentPath;
                break;
            }
        }
    }
    //if c file was found save it as cPathReport and return it
    if (cFilePath) {
        cFilePathReport->cPath = cFilePath;
        if (depth) {
            cFilePathReport->findCGradeReason = WRONG_DIRECTORY;
        } else {
            cFilePathReport->findCGradeReason = GOOD_FILE_FOUND;
        }
        cFilePathReport->depth = depth;
        CheckError(closedir(currentDir), "closing dir failed");
        return;
    }
    //if multiple dirs was found in any level, return null as the c file path
    if (multipleDirs) {
        cFilePathReport->cPath = NULL;
        cFilePathReport->findCGradeReason = MULTIPLE_DIRECTORIES;
        cFilePathReport->depth = depth;
        CheckError(closedir(currentDir), "closing dir failed");
        return;
    }
    //if nextDirPath is empty it means there no c file in the directory
    if (!strcmp(nextDirPath, "")) {
        cFilePathReport->cPath = NULL;
        cFilePathReport->findCGradeReason = NO_C_FILE;
        cFilePathReport->depth = depth;
        CheckError(closedir(currentDir), "closing dir failed");
        return;
    }
    free(innerDirentPath);
    CheckError(closedir(currentDir), "closing dir failed");
    //if we get till here it means the c file was not found in this level
    // but there is at least more level to search in it
    return FindCFile(nextDirPath, cFilePathReport, depth + 1);
}

/*******
 * func name: CompileAndRunCFile
 * input: pathToCFile - path to c file that was found in the search level.
 *        pathToInput - path to the file that contain the input for
 *                      the process of the c file.
 * output: COMPILE_AND_RUN_GRADE_REASON instance.
 *         Indicates the result of the compile and run level.
 * func operation: compile and run the c file and return a
 *                 COMPILE_AND_RUN_GRADE_REASON instance that indicates the
 *                 logical result.
 */
enum COMPILE_AND_RUN_GRADE_REASON CompileAndRunCFile(char *pathToCFile,
                                                     char *pathToInput) {
    int fdBackup;
    int fdInput;
    int sleeper = -1;
    CheckError(fdInput = open(pathToInput, O_RDONLY),
               "open input file failed");
    if ((fdBackup = dup(fdInput)) < 0) {
        perror("dup failed");
        exit(-1);
    }
    pid_t pid = fork();
    CheckError(pid, "fork failed");
    if (pid == 0) {
        //compile the c file from the search level
        char *args[] = {"gcc", pathToCFile, NULL};
        int execResult = execvp("gcc", args);
        CheckError(execResult, "execute gcc failed");
        exit(1);
    }
    if (pid > 0) {
        int compileStatus;
        //wait for the end of compilation
        CheckError(waitpid(pid, &compileStatus, 0),
                   "waiting for compilation failed");
        //if compilation never end or had error, return COMPILATION_ERROR
        if (WIFEXITED(compileStatus)) {
            if (WEXITSTATUS(compileStatus) != 0) {
                return COMPILATION_ERROR;
            }
        } else {
            return COMPILATION_ERROR;
        }
        pid = fork();
        CheckError(pid, "fork failed");
        if (pid == 0) {
            //direct the input from the input file to the input for the
            // c file's process
            CheckError(dup2(fdInput, 0), "dup2 failed");
            //running the c file's process
            CheckError(execl("a.out", "a.out", NULL), "running program failed");
        }
        if (pid > 0) {
            int status;
            int x;
            //fork another process
            CheckError(sleeper = fork(), "forking sleeper failed");
            //the sleeper only sleep for 5 seconds for counting the
            // timeout of the running
            if (sleeper == 0) {
                sleep(5);
                exit(0);
            } else {
                //waiting till one of the child process end
                while (1) {
                    x = wait(&status);
                    if (x == pid || x == sleeper)
                        break;
                }
            }
            //if the sleeper end earlier, it's timeout
            if (x == sleeper) {
                CheckError(close(fdBackup), "closing fd failed");
                //killind the timeouted process
                kill(pid, SIGKILL);
                return TIMEOUT;
            }
        }
    }
    CheckError(close(fdBackup), "closing fd failed");
    //if sleeper created and still don't end, kill it
    if (sleeper != -1) {
        kill(sleeper, SIGKILL);
    }
    //return that the compile and run worked as exepted
    return GOOD_RUN;
}

/*******
 * func name: IsDir
 * input: innerDirPath - a path to innerDirent.
 * output: 1 if the dirent is directory. else, return 0.
 * func operation: checking whether the dirent is directory.
 */
int IsDir(char *innerDirPath) {
    struct stat innerDirStat;
    // getting the stat struct of the dirent
    if (stat(innerDirPath, &innerDirStat) != 0) {
        free(innerDirPath);
        exit(-1);
    }
    //returning whether the dirent is dir
    return S_ISDIR(innerDirStat.st_mode);
}

/*******
 * func name: CheckError
 * input: x - a number represents aresult of system call.
 *        s - a string for perror if the system call failed
 * output: no output
 * func operation: checking whether the x num meant
 *                 that the sysytem call failed.
 *                 write perror and finish the program if it failed.
 */
void CheckError(int x, char *s) {
    //if the number x is -1, system call failed,
    // sothe s messege for the perror is sent
    if (x == -1) {
        perror(s);
        exit(-1);
    }
}
