/*
Ervin Pangilinan
CSC 310: File Structures & Advanced Algorithms - Spring 2023
Project 2: The jtar Software
Implementation in C++
*/

#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>
#include <utime.h>
#include <fstream>
#include <vector>
#include <map>
#include "file.cpp"
using namespace std;

typedef char String[100];
enum VERIFY {CF, TF, XF, HELP, NONE};
enum TYPE {REG, DIRECTORY};

int parseArgs(char* argv[]);
void helpMessage();
void makeTarFile(int argc, char* argv[]);
char* createFileName(string fileName);
char* createProtectionMode(string fileName);
char* createTimestamp(string fileName);
char* createFileSize(string fileName);

int main(int argc, char* argv[]) {
    // PRE: Command-line arguments are used to specify how jtar will operate.
    // POST: Executes based on the flags passed on command line.

    /* TODO: Parse command-line arguments and flag errors
    •   jtar -cf tarfile file1 dir1... 
        This specifies jtar to make a tar file named tarfile based
        on the files or directories following the name of the tarfile. All files in this assignment will
        be text files. No files will be symbolic or hard links.

    •   jtar -tf tarfile 
        This specifies jtar to list the names of all files that have packed into a tar
        file. It does not recreate anything.

    •   jtar -xf tarfile 
        This specifies jtar to read a tar file, and recreate all the files saved in that
        tarfile. This includes making any directories that must exist to hold the files. The files thus
        created should be exactly like the original files. This means that they should have the same
        access and modification times, and the same protection modes.
    */

    switch (parseArgs(argv)) {
        case CF:
            makeTarFile(argc, argv);
            break;

        case TF:
            cout << "tf flag" << endl;
            break;

        case XF:
            cout << "xf flag" << endl;
            break;

        case HELP:
            helpMessage();
            break;

        case NONE:
            cerr << "jtar: You must specify one of the options" << endl;
            cerr << "Try 'jtar --help' for more information." << endl;
            break;
    } 

   /* TODO: Flag errors
    •   An invalid format. For example, the -cf option should always have an argc of at least 4,
        and the -tf and -xf options should always have an argc of at least 3.
    •   Attempting to open a tar file which does not exist
    */


    return 0;
}

int parseArgs(char* argv[]) {
    // PRE: Flags are passed on the command line.
    // POST: Returns an integer determining which command should be executed.

    string flag(argv[1]);
    if (flag == "-cf") {
        return CF;
    } else if (flag == "-tf") {
        return TF;
    } else if (flag == "-xf") {
        return XF;
    } else if (flag == "--help") {
        return HELP;
    } else {
        return NONE;
    }
}

void helpMessage() {
    // PRE: "--help" was passed on the command line.
    // POST: Outputs the help message for jtar. 

    cout << "'jtar' saves many files together into a single tape or disk archive, and" << endl;
    cout << "can restore individual files from the archive." << endl;
    cout << "\nUsage: tar [OPTION]... [FILE]..." << endl;
    cout << "\nOption Examples:" << endl;
    cout << "\ttar -cf archive.tar foo bar\t# Create archive.tar from files foo and bar." << endl;
    cout << "\ttar -tf archive.tar\t\t# List all files in archive.tar verbosely." << endl;
    cout << "\ttar -xf archive.tar\t\t# Extract all files from archive.tar." << endl;
    cout << "Report bugs to <11013518@live.mercer.edu>.\n" << endl; 
}

void makeTarFile(int argc, char* argv[]) {
    // PRE: "-cf" flag was passed onto the command line.
    // POST: Makes a tar file with the passed arguments.

    map<string, int> files;
    vector<File> tarredFiles;

    if (argc >= 4) {
        // Step 1: Determine the type of each argument passed after the tar file name.
        struct stat buf;
        struct utimbuf timebuf;
        for (int i = 3; i < argc; i++) {
            lstat(argv[i], &buf);
            string name(argv[i]);
            if (S_ISREG(buf.st_mode)) {
                files.insert({name, REG});
                cout << createFileName(name) << "\t" << createProtectionMode(name) << "\t" << createTimestamp(name) << "\t" << createFileSize(name) << endl;
            } else if (S_ISDIR(buf.st_mode)) {
                cout << createFileName(name) << "\t" << createProtectionMode(name) << "\t" << createTimestamp(name) << "\t" << createFileSize(name) << endl;
                files.insert({name, DIRECTORY});
            } else {
                cerr << "jtar: '" << argv[i] << "' does not exist" << endl;
                return;
            }
        }



    } else {
        cerr << "jtar: Invalid format" << endl;
        cerr << "Try 'jtar --help for more information" << endl;
    }
}

char* createFileName(string fileName) {
    // PRE: String containing file name is passed in.
    // POST: Returns a char array containing the file name.

    char *name = new char[81];
    strcpy(name, fileName.c_str());
    return name;
}

char* createProtectionMode(string fileName) {
    // PRE: String containing file name is passed in.
    // POST: Returns a char array containing the protection mode of file.

    struct stat buf;
    char* pmode = new char[5];
    lstat(fileName.c_str(), &buf);
    string fileProtect = to_string(((buf.st_mode & S_IRWXU) >> 6)) 
                       + to_string((buf.st_mode & S_IRWXG) >> 3) 
                       + to_string(buf.st_mode & S_IRWXO);

    strcpy(pmode, fileProtect.c_str());
    return pmode;
}

char* createTimestamp(string fileName) {
    // PRE: String containing file name is passed in.
    // POST: Returns a char array containing the timestamp of the file.

    struct stat buf;
    char* timestamp = new char[16];
    lstat(fileName.c_str(), &buf);
    strftime(timestamp, 16, "%Y%m%d%H%M.%S", localtime(&buf.st_mtime));
    return timestamp;

}

char* createFileSize(string fileName) {
    // PRE: String containing file name is passed in.
    // POST: Returns a char array containing the file size.

    struct stat buf;
    char *size = new char[7];
    lstat(fileName.c_str(), &buf);
    string fileSize = to_string(buf.st_size);
    strcpy(size, fileSize.c_str());
    return size;
}
