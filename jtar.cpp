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
#include "file.cpp"
using namespace std;

typedef char String[100];
enum VERIFY {CF, TF, XF, HELP, NONE};
enum TYPE {FILE, DIRECTORY};

int parseArgs(char* argv[]);
void helpFlag();
void makeTarFile();
int findType();

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

    •   jtar --help 
        If the help option is present on the command line, the program should print a
        description of the three options above, and exit.
    */

    switch (parseArgs(argv)) {
        case CF:
            cout << "cf flag" << endl;
            break;

        case TF:
            cout << "tf flag" << endl;
            break;

        case XF:
            cout << "xf flag" << endl;
            break;

        case HELP:
            helpFlag();
            break;

        case NONE:
            cerr << "jtar: You must specify one of the options" << endl;
            cerr << "Try 'jtar --help' for more information." << endl;
            break;
    } 

   /* TODO: Flag errors
    •   Failure to specify one of the four options supported by jtar
    •   An invalid option on the command line
    •   An invalid format. For example, the -cf option should always have an argc of at least 4,
        and the -tf and -xf options should always have an argc of at least 3.
    •   Attempting to tar a file or directory which does not exist
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

void helpFlag() {
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

    if (argc >= 4) {

    } else {
        cerr << "jtar: Invalid format" << endl;
        cerr << "Try 'jtar --help for more information" << endl;
    }
}
