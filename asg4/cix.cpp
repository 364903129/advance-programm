// $Id: cix.cpp,v 1.9 2019-04-05 15:04:28-07 - - $
//Haofan Wang (hwang108@ucsc.edu)
//Qiutong Li (qli33@ucsc.edu)

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
using namespace std;

#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>

#include "protocol.h"
#include "logstream.h"
#include "sockets.h"

logstream outlog (cout);
struct cix_exit: public exception {};

unordered_map<string,cix_command> command_map {
        {"exit", cix_command::EXIT},
        {"help", cix_command::HELP},
        {"ls"  , cix_command::LS  },
        {"get", cix_command::GET},
        {"rm", cix_command::RM},
        {"put", cix_command::PUT},
};

static const char help[] = R"||(
exit         - Exit the program.  Equivalent to EOF.
get filename - Copy remote file to local host.
help         - Print help summary.
ls           - List names of files on remote server.
put filename - Copy local file to remote host.
rm filename  - Remove file from remote server.
)||";

void cix_help() {
    cout << help;
}

void cix_ls (client_socket& server) {
    cix_header header;
    header.command = cix_command::LS;
    outlog << "sending header " << header << endl;
    send_packet (server, &header, sizeof(header));
    recv_packet (server, &header, sizeof(header));
    outlog << "received header " << header << endl;
    if (header.command != cix_command::LSOUT) {
        outlog << "sent LS, server did not return LSOUT" << endl;
        outlog << "server returned " << header << endl;
    }else {
        auto buffer = std::make_unique<char[]> (header.nbytes + 1);
        recv_packet (server, buffer.get(), header.nbytes);
        outlog << "received " << header.nbytes << " bytes" << endl;
        buffer[header.nbytes] = '\0';
        cout << buffer.get();
    }
}

void cix_get(client_socket& server) {
    string line;
    cin >> line;
    if (cin.eof()) throw cix_exit();

    cix_header header;
    header.command = cix_command::GET;
    strcpy(header.filename, line.c_str());
    outlog << "sending header " << header << endl;
    send_packet (server, &header, sizeof(header));
    recv_packet (server, &header, sizeof(header));
    outlog << "received header " << header << endl;
    if (header.command != cix_command::FILEOUT) {
        outlog << "sent GET, server did not return GETOUT" << endl;
        outlog << "server returned " << header << endl;
    }else {
        auto buffer = std::make_unique<char[]> (header.nbytes);
        recv_packet (server, buffer.get(), header.nbytes);
        outlog << "received " << header.nbytes << " bytes file" << endl;
        std::ofstream outfile (header.filename, std::ofstream::binary);
        outfile.write (buffer.get(), header.nbytes);
        outfile.close();
        cout << "OK\n";
    }
}
void cix_rm(client_socket& server) {
    string line;
    cin >> line;
    if (cin.eof()) throw cix_exit();
    cix_header header;
    header.command = cix_command::RM;
    strcpy(header.filename, line.c_str());
    outlog << "sending header " << header << endl;
    send_packet (server, &header, sizeof(header));
    recv_packet (server, &header, sizeof(header));
    outlog << "received header " << header << endl;
    if (header.command != cix_command::ACK) {
        outlog << "sent RM, server did not return ACK" << endl;
        outlog << "server returned " << header << endl;
    }else {
        cout << "OK\n";
    }
}

void cix_put(client_socket& server) {
    string line;
    while(line == "") {
        cin >> line;
    }
    if (cin.eof()) throw cix_exit();
    cix_header header;
    strcpy(header.filename, line.c_str());
    std::ifstream is (line, std::ifstream::binary);
    if (is) {
        // get length of file:
        is.seekg (0, is.end);
        int length = is.tellg();
        is.seekg (0, is.beg);
        char * buffer = new char [length];
        // read data as a block:
        is.read (buffer,length);
        is.close();
        header.nbytes = length;
        header.command = cix_command::PUT;
        outlog << "sending header " << header << endl;
        send_packet (server, &header, sizeof(header));
        send_packet (server, buffer, length);
        delete[] buffer;
    } else {
        cout << line << " not exist!\n";
        return;
    }
    recv_packet (server, &header, sizeof(header));
    outlog << "received header " << header << endl;
    if (header.command != cix_command::ACK) {
        outlog << "sent PUT, server did not return ACK" << endl;
        outlog << "server returned " << header << endl;
    }else {
        cout << "OK\n";
    }
}

void usage() {
    cerr << "Usage: " << outlog.execname() << " [host] [port]" << endl;
    throw cix_exit();
}

int main (int argc, char** argv) {
    outlog.execname (basename (argv[0]));
    outlog << "starting" << endl;
    vector<string> args (&argv[1], &argv[argc]);
    if (args.size() > 2) usage();
    string host = get_cix_server_host (args, 0);
    in_port_t port = get_cix_server_port (args, 1);
    outlog << to_string (hostinfo()) << endl;
    try {
        outlog << "connecting to " << host << " port " << port << endl;
        client_socket server (host, port);
        outlog << "connected to " << to_string (server) << endl;
        for (;;) {
            string line;
            cin >> line;
            if (cin.eof()) throw cix_exit();
            outlog << "command " << line << endl;
            const auto& itor = command_map.find (line);
            cix_command cmd = itor == command_map.end()
                              ? cix_command::ERROR : itor->second;
            switch (cmd) {
                case cix_command::EXIT:
                    throw cix_exit();
                    break;
                case cix_command::HELP:
                    cix_help();
                    break;
                case cix_command::LS:
                    cix_ls(server);
                    break;
                case cix_command::GET:
                    cix_get(server);
                    break;
                case cix_command::RM:
                    cix_rm(server);
                    break;
                case cix_command::PUT:
                    cix_put(server);
                    break;
                default:
                    outlog << line << ": invalid command" << endl;
                    break;
            }
        }
    }catch (socket_error& error) {
        outlog << error.what() << endl;
    }catch (cix_exit& error) {
        outlog << "caught cix_exit" << endl;
    }
    outlog << "finishing" << endl;
    return 0;
}
