// $Id: file_sys.cpp,v 1.7 2019-07-11 12:30:13-07 - - $
//Partner: Haofan Wang (hwang108@ucsc.edu)
//Partner: Qiutong Li (qli33@ucsc.edu)

#include <iostream>
#include <stdexcept>
#include <unordered_map>

using namespace std;

#include "debug.h"
#include "file_sys.h"

size_t inode::next_inode_nr {1};

struct file_type_hash {
   size_t operator() (file_type type) const {
      return static_cast<size_t> (type);
   }
};

ostream& operator<< (ostream& out, file_type type) {
   static unordered_map<file_type,string,file_type_hash> hash {
      {file_type::PLAIN_TYPE, "PLAIN_TYPE"},
      {file_type::DIRECTORY_TYPE, "DIRECTORY_TYPE"},
   };
   return out << hash[type];
}

inode_state::inode_state() {
   DEBUGF ('i', "root = " << root << ", cwd = " << cwd
          << ", prompt = \"" << prompt() << "\"");
   this->root = make_shared<inode>(inode(file_type::DIRECTORY_TYPE));
   this->root->contents->set_abs_path("/");
   this->root->contents->get_dirents().insert(
      pair<string, inode_ptr>(".", this->root));
   this->root->contents->get_dirents().insert(
      pair<string, inode_ptr>("..", this->root));
   this->cwd = this->root;
}

const string& inode_state::prompt() const { return prompt_; }

void inode_state::set_prompt(const string& new_prompt) {
   this->prompt_ = new_prompt + " ";
} 

inode_ptr inode_state::find_inode(string pathname) const {
   if (pathname == "/")
        return this->root;
   if (pathname == ".")
      return this->cwd;
   if (pathname == "..")
      return this->cwd->get_contents()->get_dirents()[".."];

   inode_ptr ret = this->cwd;
   if (pathname[0] == '/')
      ret = this->root;
   wordvec paths = split(pathname, "/");
   bool is_start = false;
   for (auto path: paths) {
      if (path.length() == 0 && is_start) {
         return nullptr;
      }
      if (path.length() > 2 && !is_start) {
         is_start = true;
      }
      try {
         auto iter = ret->get_contents()->get_dirents().find(path);
         if (iter != ret->get_contents()->get_dirents().end()) {
            ret = iter->second;
         }
         else {
            return nullptr;
         }
      }
      catch(file_error&) {
         return nullptr;
      }
   }
   return ret;
}

inode_ptr inode_state::get_root() const {
   return this->root;
}

inode_ptr inode_state::get_cwd() const {
   return this->cwd;
}

void inode_state::set_cwd(inode_ptr new_inode) {
   this->cwd = new_inode;
}

ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

inode::inode(file_type type): inode_nr (next_inode_nr++) {
   switch (type) {
      case file_type::PLAIN_TYPE:
           contents = make_shared<plain_file>();
           break;
      case file_type::DIRECTORY_TYPE:
           contents = make_shared<directory>();
           break;
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

int inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}

void inode::ls(bool recurrsive) const {
   if (this->contents->type() != "directory") {
      return;
   }

   cout << this->contents->get_abs_path() << " :" << endl;

   for (auto x: this->contents->get_dirents()) {
      cout << "\t" << x.second->get_inode_nr()
           << "\t" << x.second->get_contents()->size() << "    "
           << x.first;
      if (x.second->get_contents()->type() == "directory" &&
          x.first != ".." && x.first != ".")
         cout << "/";
      cout << endl;
   }

   if (recurrsive) {
      for (auto x: this->contents->get_dirents()) {
         if (x.first != "." && x.first != ".." &&
             x.second->get_contents()->type() == "directory") {
            x.second->ls(true);
         }
      }
   }
}

base_file_ptr inode::get_contents() {
   return this->contents;
}

file_error::file_error (const string& what):
            runtime_error (what) {
}

size_t plain_file::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   if (this->data.size() > 0) {
      for (auto word: this->data){
         size += word.length();
      }
      size += this->data.size() - 1;
   }
   return size;
}

const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

void plain_file::writefile (const wordvec& words) {
   DEBUGF ('i', words);
}

void plain_file::remove (const string&) {
   throw file_error ("is a plain file");
}

void plain_file::remove_recurrsive (const string&) {
   throw file_error ("is a plain file");
}

inode_ptr plain_file::mkdir (const string&) {
   throw file_error ("is a plain file");
}

inode_ptr plain_file::mkfile (const string&) {
   throw file_error ("is a plain file");
}

wordvec& plain_file::get_data() {
   return this->data;
}

map<string, inode_ptr>& plain_file::get_dirents() {
   throw file_error ("is a plain file");
}

string plain_file::type() const{
   return "plain_file";
}

string plain_file::get_abs_path() const {
   return this->abs_path;
}

void plain_file::set_abs_path(string path) {
   this->abs_path = path;
}

size_t directory::size() const {
   size_t size {0};
   size = this->dirents.size();
   DEBUGF ('i', "size = " << size);
   return size;
}

const wordvec& directory::readfile() const {
   throw file_error ("is a directory");
}

void directory::writefile (const wordvec&) {
   throw file_error ("is a directory");
}

void directory::remove (const string& filename) {
   DEBUGF ('i', filename);
   auto iter = this->dirents.find(filename);
   if (iter == this->dirents.end()) {
      cout << "remove: No such file" << endl;
      return;
   }
   if (iter->second->get_contents()->type() == "directory" &&
       iter->second->get_contents()->get_dirents().size() > 2) {
      cout << "remove: Directory is not empty" << endl;
      return;
   }
   this->dirents.erase(filename);
}

void directory::remove_recurrsive (const string& filename) {
   DEBUGF ('i', filename);
   auto iter = this->dirents.find(filename);
   if (iter == this->dirents.end()) {
      cout << "remove: No such file" << endl;
      return;
   }
   if (iter->second->get_contents()->type() == "directory") {
      for (auto x: iter->second->get_contents()->get_dirents()) {
         if (x.first != "." && x.first != "..") {
            iter->second->get_contents()->remove_recurrsive(x.first);
         }
      }
   }
   this->dirents.erase(filename);
}

inode_ptr directory::mkdir (const string& dirname) {
   DEBUGF ('i', dirname);
   if (this->dirents.find(dirname) != this->dirents.end()) {
      cout << "mkdir: Directory alread exists" << endl;
      return nullptr;
   }
   inode_ptr new_inode = make_shared<inode>(
      inode(file_type::DIRECTORY_TYPE));
   if (this->abs_path == "/") {
      new_inode->get_contents()->set_abs_path(
         this->abs_path + dirname);
   }
   else {
      new_inode->get_contents()->set_abs_path(
         this->abs_path + "/" + dirname);
   }
   this->dirents[dirname] = new_inode;
   return new_inode;
}

inode_ptr directory::mkfile (const string& filename) {
   DEBUGF ('i', filename);
   inode_ptr new_inode = make_shared<inode>(
      inode(file_type::PLAIN_TYPE));
   auto iter = this->dirents.find(filename);
   if (iter != this->dirents.end() &&
       iter->second->get_contents()->type() == "directory") {
      cout << "mkfile: Already exists directory" << endl;
      return nullptr;
   }
   if (this->abs_path == "/") {
      new_inode->get_contents()->set_abs_path(
         this->abs_path + filename);
   }
   else {
      new_inode->get_contents()->set_abs_path(
         this->abs_path + "/" + filename);
   }
   this->dirents[filename] = new_inode;
   return new_inode;
}

wordvec& directory::get_data() {
   throw file_error ("is a directory");
}

map<string, inode_ptr>& directory::get_dirents() {
   return this->dirents;
}

string directory::type() const {
   return "directory";
}

string directory::get_abs_path() const {
   return this->abs_path;
}

void directory::set_abs_path(string path) {
   this->abs_path = path;
}
