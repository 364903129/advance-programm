// $Id: commands.cpp,v 1.17 2018-01-25 14:02:55-08 - - $
//Partner: Haofan Wang (hwang108@ucsc.edu)
//Partner: Qiutong Li (qli33@ucsc.edu)


#include "commands.h"
#include "debug.h"

command_hash cmd_hash {
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },
   {"lsr"   , fn_lsr   },
   {"make"  , fn_make  },
   {"mkdir" , fn_mkdir },
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },
   {"rmr"   , fn_rmr   },
};

command_fn find_command_fn (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   DEBUGF ('c', "[" << cmd << "]");
   const auto result = cmd_hash.find (cmd);
   if (result == cmd_hash.end()) {
      throw command_error (cmd + ": no such function");
   }
   return result->second;
}

command_error::command_error (const string& what):
            runtime_error (what) {
}

int exit_status_message() {
   int exit_status = exit_status::get();
   cout << execname() << ": exit(" << exit_status << ")" << endl;
   return exit_status;
}

void fn_cat (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   if (words.size() < 2) {
      cout << "cat: missing argument" << endl;
      return;
   }

   for (size_t i = 1; i < words.size(); i++) {
      inode_ptr iptr = state.find_inode(words[i]);
      if (iptr == nullptr || 
          iptr->get_contents()->type() == "directory") {
         cout << "cat: cannot find file " << words[i] << endl;
      }
      else {
         auto data = iptr->get_contents()->get_data();
         for (size_t j = 0; j < data.size(); j++) {
            cout << data[j];
            if (j < data.size() - 1) {
               cout << " ";
            }
         }
      }
   }
   cout << endl;
}

void fn_cd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   
   inode_ptr iptr;
   if (words.size() < 2) {
      iptr = state.find_inode("/");
   }
   else {
      iptr = state.find_inode(words[1]);
   }

   if (iptr == nullptr) {
      cout << "Directory not found" << endl;
      return;
   }
   if (iptr->get_contents()->type() != "directory"){
      cout << "Not a directory" << endl;
      return;
   }
   
   state.set_cwd(iptr);
}

void fn_echo (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << word_range (words.cbegin() + 1, words.cend()) << endl;
}


void fn_exit (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   if (words.size() < 2) {
      exit_status::set(0);
      throw ysh_exit();
   }

   if(words[1].empty() || 
      ((!isdigit(words[1][0])) &&
       (words[1][0] != '-') && 
       (words[1][0] != '+'))) {
      exit_status::set(127);
      throw ysh_exit();
   }

   char *p;
   strtol(words[1].c_str(), &p, 10);

   if (*p == 0) {
      exit_status::set(atoi(words[1].c_str()));
      throw ysh_exit();
   }
   exit_status::set(0);
   throw ysh_exit();
}

void fn_ls (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   inode_ptr iptr;
   if (words.size() < 2) {
      iptr = state.find_inode("./");
   }
   else {
      iptr = state.find_inode(words[1]);
   }

   if (iptr == nullptr){
      cout << "Invalid pathname" << endl;
      return;
   }
   if (iptr->get_contents()->type() != "directory") {
      cout << "Not a directory" << endl;
      return;
   }

   iptr->ls(false);
}

void fn_lsr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   inode_ptr iptr;
   if (words.size() < 2) {
      iptr = state.find_inode("./");
   }
   else {
      iptr = state.find_inode(words[1]);
   }

   if (iptr == nullptr){
      cout << "Invalid pathname" << endl;
      return;
   }
   if (iptr->get_contents()->type() != "directory") {
      cout << "Not a directory" << endl;
      return;
   }

   iptr->ls(true);
}

void fn_make (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   if (words.size() < 2){
      cout << "make: missing argument" << endl;
      return;
   }

   string pathname = words[1];
   wordvec paths = split(pathname, "/");
   string dir("");
   if (pathname[0] == '/') {
      dir += "/";
   }
   for (unsigned int i = 0; i < paths.size() - 1; i++)
      dir += paths[i] + "/";

   inode_ptr dir_inode = state.find_inode(dir);
   if (dir_inode == nullptr || 
         dir_inode->get_contents()->type() != "directory"){
      cout << "make: No such file or directory" << endl;
      return;
   }

   string filename = paths[paths.size() - 1];
   inode_ptr file_inode = dir_inode->get_contents()->mkfile(filename);

   for(size_t i = 2; i < words.size(); i++) {
      file_inode->get_contents()->get_data().push_back(words[i]);
   }
}

void fn_mkdir (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   if (words.size() < 2){
      cout << "make: missing argument" << endl;
      return;
   }

   string pathname = words[1];
   wordvec paths = split(pathname, "/");
   string dir("");
   if (pathname[0] == '/') {
      dir += "/";
   }
   for (unsigned int i = 0; i < paths.size() - 1; i++)
      dir += paths[i] + "/";

   inode_ptr dir_inode = state.find_inode(dir);
   if (dir_inode == nullptr || 
         dir_inode->get_contents()->type() != "directory"){
      cout << "mkdir: No such file or directory" << endl;
      return;
   }

   string new_dirname = paths[paths.size() - 1];
   inode_ptr new_dir_inode = dir_inode
      ->get_contents()->mkdir(new_dirname);
   if (new_dir_inode == nullptr) {
      cout << "mkdir: Directory already exists" << endl;
      return;
   }
   new_dir_inode->get_contents()->get_dirents().insert(
      pair<string, inode_ptr>(".", new_dir_inode));
   new_dir_inode->get_contents()->get_dirents().insert(
      pair<string, inode_ptr>("..", dir_inode));
}

void fn_prompt (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if (words.size() < 2) {
      cout << "prompt: missing argument" << endl;
      return;
   }
   string new_prompt("");
   for (size_t i = 1; i < words.size(); i++) {
      new_prompt += words[i];
      if (i < words.size() - 1) {
         new_prompt += " ";
      }
   }
   state.set_prompt(new_prompt);
}

void fn_pwd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << state.get_cwd()->get_contents()->get_abs_path() << endl;
}

void fn_rm (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   if (words.size() < 2){
      cout << "rm: missing argument" << endl;
      return;
   }

   string pathname = words[1];
   wordvec paths = split(pathname, "/");
   string dir("");
   if (pathname[0] == '/') {
      dir += "/";
   }
   for (unsigned int i = 0; i < paths.size() - 1; i++)
      dir += paths[i] + "/";

   inode_ptr dir_inode = state.find_inode(dir);
   if (dir_inode == nullptr || 
         dir_inode->get_contents()->type() != "directory"){
      cout << "make: No such file or directory" << endl;
      return;
   }

   string filename = paths[paths.size() - 1];
   dir_inode->get_contents()->remove(filename);
   
}

void fn_rmr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   if (words.size() < 2){
      cout << "rm: missing argument" << endl;
      return;
   }

   string pathname = words[1];
   wordvec paths = split(pathname, "/");
   string dir("");
   if (pathname[0] == '/') {
      dir += "/";
   }
   for (unsigned int i = 0; i < paths.size() - 1; i++)
      dir += paths[i] + "/";

   inode_ptr dir_inode = state.find_inode(dir);
   if (dir_inode == nullptr || 
         dir_inode->get_contents()->type() != "directory"){
      cout << "make: No such file or directory" << endl;
      return;
   }

   string filename = paths[paths.size() - 1];
   dir_inode->get_contents()->remove_recurrsive(filename);
}
