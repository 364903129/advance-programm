// pair-programming by Qiutong Li(qli33) & Haofan Wang (hwang108)
// $Id: main.cpp,v 1.11 2018-01-25 14:19:29-08 - - $

#include <cstdlib>
#include <exception>
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>

using namespace std;

#include "listmap.h"
#include "xpair.h"
#include "util.h"

using str_str_map = listmap<string,string>;
using str_str_pair = str_str_map::value_type;

void scan_options(int argc, char** argv) {
    opterr = 0;
    for (;;) {
        int option = getopt(argc, argv, "@:");
        if (option == EOF)
            break;
        switch (option) {
        case '@':
            debugflags::setflags(optarg);
            break;
        default:
            complain() << "-" << char(optopt) 
            << ": invalid option" << endl;
            break;
        }
    }
}

int count = 1;
str_str_map ssmap;

bool contains(string line, char ch) {
    for (size_t i = 0; i < line.size(); i++) {
        if (ch == line[i]) {
            return true;
        }
    }
    return false;
}

int indexOf(string line, char ch) {
    for (size_t i = 0; i < line.size(); i++) {
        if (ch == line[i]) {
            return i;
        }
    }
    return -1;
}

void processLine(str_str_map& test, string filename, string line) {
    //cout << line << endl;
    cout << filename << ": " << count << ": " << line << endl;
    count++;

    if(line.size() == 0 || line[0] == ' '){
        return;
    }

    if (line[0] == '#') {
        return;
    }

    if (contains(line, '=')) {
        int index = indexOf(line, '=');
        string key = line.substr(0, index);
        if (key == "") {
            if (line.size() == 1) {
                for (str_str_map::iterator itor = test.begin();
                        itor != test.end(); ++itor) {
                    cout << itor->first << " = " 
                    << itor->second << endl;
                }
            } else {
                string value = line.substr(index + 1,
                        line.size() - key.size() - 1);

                for (str_str_map::iterator itor = test.begin();
                        itor != test.end(); ++itor) {
                    if (itor->second == value) {
                        cout << itor->first << " = " 
                        << itor->second << endl;
                    }
                }
            }
        } else {
            /*
            if (contains(key, ' ')) {
                cerr << "error line: " << line << endl;
                return;
            }
            */

            string value = line.substr(index + 1, 
            line.size() - key.size() - 1);
            cout << key << " = " << value << endl;
            test.insert(str_str_pair(key, value));
        }
    } else {
        /*
        if (contains(line, ' ')) {
            cerr << "error line: " << line << endl;
            return;
        }
        */

        string key = line;
        if (test.find(key) == test.end()) {
            cout << key << ": key not found" << endl;
        } else {
            cout << key << " = " 
            << test.find(key)->second << endl;
        }
    }
}

void processFile(string filename, istream& is) {
    string line;


    count = 1;
    getline(is, line);
    while (!is.eof()) {
        processLine(ssmap, filename, line);
        getline(is, line);
    }
}

int main(int argc, char** argv) {
    sys_info::execname(argv[0]);
    scan_options(argc, argv);

    int fcount = 0;
    for (char** argp = &argv[optind]; argp != &argv[argc]; ++argp) {
        str_str_pair pair(*argp, to_string<int>(argp - argv));
        fcount++;
        if (pair.first == "-") {
            processFile("-", cin);
        } else {
            ifstream ifs(pair.first.c_str());
            if (ifs.is_open()) {
                processFile(pair.first, ifs);
                ifs.close();
            }else{
                cout << "keyvalue: " 
                << pair.first 
                << ": No such file or directory" 
                << endl;
            }
        }
    }

    if(fcount == 0){
        processFile("-", cin);
    }

    /*
     for (str_str_map::iterator itor = test.begin();
     itor != test.end(); ++itor) {
     cout << "During iteration: " << *itor << endl;
     }

     str_str_map::iterator itor = test.begin();
     test.erase (itor);

     for (str_str_map::iterator itor = test.begin();
     itor != test.end(); ++itor) {
     cout << "During iteration: " << *itor << endl;
     }

     cout << "EXIT_SUCCESS" << endl;
     */

    return EXIT_SUCCESS;
}

