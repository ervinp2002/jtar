/*
Ervin Pangilinan
CSC 310: File Structures & Advanced Algorithms - Spring 2023
Project 2: The jtar Software
Implementation in C++
*/

#include <iostream>
#include <string>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/types.h>
#include <utime.h>
#include <fstream>
#include <vector>
#include <map>
#include <sstream>
#include "file.h"
using namespace std;

// For initial command line parsing.
enum VERIFY {CF, TF, XF, HELP, NONE};
int parseArgs(char* argv[]);

// For "--help" flag. 
void helpMessage();

// For "-cf" flag.
void makeTarFile(int argc, char* argv[]);
char* createFileName(string fileName);
char* createProtectionMode(string fileName);
char* createTimestamp(string fileName);
char* createFileSize(string fileName);
bool isDirectory(string fileName);
void obtainFiles(int argc, char* argv[], vector<string> &listing);
void setUpFiles(const vector<string> &listing, vector<File> &tarTheseFiles);
void build(vector<File> &tarTheseFiles, char* tarName);

// For "-tf" flag.
void listContents(char* tarredFile);

// For "-xf" flag.
void untar(char* tarredFile);

int main(int argc, char* argv[]) {
    // PRE: Command-line arguments are used to specify how jtar will operate.
    // POST: Executes based on the flags passed on command line.

    switch (parseArgs(argv)) {
        case CF:
            makeTarFile(argc, argv);
            break;

        case TF:
            listContents(argv[2]);
            break;

        case XF:
            untar(argv[2]);
            break;

        case HELP:
            helpMessage();
            break;

        case NONE:
            cerr << "jtar: You must specify one of the options" << endl;
            cerr << "Try 'jtar --help' for more information." << endl;
            break;
    } 

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
    vector<string> listing;
    vector<File> tarredFiles;

    if (argc >= 4) {
        // Step 1: Verify that valid files and directories were passed onto the command line.
        struct stat buf;
        struct utimbuf timebuf;

        for (int i = 3; i < argc; i++) {
            lstat(argv[i], &buf);
            string name(argv[i]);
            fstream temp(argv[i], ios::in);

            if (!S_ISREG(buf.st_mode) && !S_ISDIR(buf.st_mode) && !temp) {
                cerr << "jtar: '" << argv[i] << "' does not exist" << endl;
                return;
            }
        }

        // Step 2: Obtain all of the files and directories to be placed into the tar file.
        obtainFiles(argc, argv, listing);

        // Step 3: Fill the vector of File objects.
        setUpFiles(listing, tarredFiles);

        // Step 4: Build the actual tar file.
        build(tarredFiles, argv[2]);
        
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

bool isDirectory(string fileName) {
    // PRE: String containing file name is passed in. 
    // POST: Returns an boolean determining if the file is a directory or not

    struct stat buf;
    lstat(fileName.c_str(), &buf);
    if (S_ISDIR(buf.st_mode)) return true;
    else return false;
}

void obtainFiles(int argc, char* argv[], vector<string> &listing) {
    // PRE: Command line arguments contain valid file and directory names.
    // POST: Obtains all files and subdirectories in current directory.

    string name;
    struct stat buf;
    for (int i = 3; i < argc; i++) {
        name = argv[i];
        if (isDirectory(name)) {
            // Add directory name to vector.
            listing.push_back(name);
            string command = "ls " + string(name) + " -1R > listOutput";

            // Make a file containing a list of files in directory name.
            system(command.c_str());
            fstream list("listOutput", ios::in);
            list.seekg(0, ios::beg);
            
            // Go through that file to add the other files.
            string fromList;
            string path = "";
            list >> fromList;
            while (!list.eof()) {
                if (fromList[fromList.length() - 1] == ':') {
                    path = fromList.substr(0, fromList.length() - 1);
                } else {
                    listing.push_back(path + "/" + fromList);
                }

                list >> fromList;
            }

            system("rm listOutput");

        } else {
            listing.push_back(name);
        }
    }
}

void setUpFiles(const vector<string> &listing, vector<File> &tarTheseFiles) {
    // PRE: Vector of filename strings is non-empty.
    // POST: Fills a new vector of File objects.

    for (string filename : listing) {
        if (isDirectory(filename)) {
            File tarredFile(createFileName(filename), createProtectionMode(filename), 
                            createFileSize(filename), createTimestamp(filename));
            tarredFile.flagAsDir();
            tarTheseFiles.push_back(tarredFile); 
        } else {
            File tarredFile(createFileName(filename), createProtectionMode(filename), 
                            createFileSize(filename), createTimestamp(filename));
            tarTheseFiles.push_back(tarredFile);
        }  
    }
}

void build(vector<File> &tarTheseFiles, char* tarName) {
    // PRE: Vector of File objects is filled.
    // POST: Creates a tar file from the vector of File objects.
    
    fstream tarFile(tarName, ios::out | ios::binary);
    struct stat buf;

    // Write out the size of the tar file. 
    unsigned long tarSize = 0;
    for (int i = 0; i < tarTheseFiles.size(); i++) {
        lstat(tarTheseFiles[i].getName().c_str(), &buf);
        if (S_ISDIR(buf.st_mode)) {
            tarSize += sizeof(File);
        } else {
            tarSize += (sizeof(File) + (unsigned long) stoi(tarTheseFiles[i].getSize()));
        }
    }
    tarFile.write((char*) &tarSize, sizeof(unsigned long));

    // Write out the contents of each file.
    for (int i = 0; i < tarTheseFiles.size(); i++) {
        File temp(tarTheseFiles[i]);
        lstat(temp.getName().c_str(), &buf);
        if (S_ISDIR(buf.st_mode)) {
            tarFile.write((char*) &temp, sizeof(File));
        } else {
            char ch;
            tarFile.write((char*) &temp, sizeof(File));

            fstream textContents(temp.getName(), ios::in);
            textContents.seekg(0, ios::beg);
            while (textContents.get(ch)) {
                tarFile.put(ch);
            }
            
            textContents.close();
        }   
    }
    
    tarFile.close();
}

void listContents(char* tarredFile) {
    // PRE: Valid tar file was passed onto the command line.
    // POST: Lists all files and directories that were written into the tar file.

    struct stat buf;
    lstat(tarredFile, &buf);
    if (!S_ISREG(buf.st_mode) && !S_ISDIR(buf.st_mode)) {
        cerr << "jtar: '" << tarredFile << "' does not exist" << endl;
    } else {
        File temp;
        unsigned long size;
        fstream tarFile(tarredFile, ios::in | ios::binary);
        tarFile.read((char*) &size, sizeof(unsigned long));
        
        while (tarFile.read((char*) &temp, sizeof(File)) && tarFile.good()) {
            if (temp.isADir()) {
                cout << temp.getName() << endl;
            } else {
                cout << temp.getName() << endl;
                tarFile.seekg(stoi(temp.getSize()), ios::cur);
            }
        }

        tarFile.close();
    }
}

void untar(char* tarredFile) {
    // PRE: "-xf" is passed onto the command line, followed by the tar file name.
    // POST: Extracts all files contained in the tar file.

    char ch;
    File temp;
    string command;
    string chmod;
    unsigned long size;
    struct stat buf;
    fstream tarFile(tarredFile, ios::in | ios::binary);
    tarFile.read((char*) &size, sizeof(unsigned long));
    
    while (tarFile.read((char*) &temp, sizeof(File)) && tarFile.good()) {
        if (temp.isADir()) {
            command = "mkdir " + temp.getName();
            system(command.c_str());
        } else {
            fstream outputFile(temp.getName(), ios::out);
            while (tarFile.get(ch) && outputFile.tellp() < (unsigned long) (stoi(temp.getSize()) - 1)) {
                outputFile.put(ch);
            }

            outputFile.close(); 
            command = "chmod " + temp.getPmode() + " ./" + temp.getName();
            system(command.c_str());            
            command = "touch -t " + temp.getStamp() + " ./" + temp.getName();
            system(command.c_str());    
        }
    }
}
