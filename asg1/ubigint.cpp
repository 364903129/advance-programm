// $Id: ubigint.cpp,v 1.8 2020-01-06 13:39:55-08 - - $
//haofan wang hwang108@ucsc.edu
//Qihang Huang qhuang24@ucsc.edu

#include <cctype>
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <climits>
using namespace std;

#include "ubigint.h"
#include "debug.h"
#include "util.h"

ubigint::ubigint (unsigned long that){
   DEBUGF ('~', this << " -> " << that);
   string string_that = to_string(that);
   for (char digit: string_that) {
      if (not isdigit (digit)) {
         throw invalid_argument ("ubigint::ubigint("+string_that+")");
      }
      ubig_value.push_back(digit - '0');
   }
}

ubigint::ubigint (const string& that) {
   DEBUGF ('~', "that = \"" << that << "\"");
   for (char digit: that) {
      if (not isdigit (digit)) {
         throw invalid_argument ("ubigint::ubigint(" + that + ")");
      }
      ubig_value.push_back(digit-'0');
   }
   reverse(ubig_value.begin(), ubig_value.end()); 
}

ubigint ubigint::operator+ (const ubigint& that) const {
   int carry = 0;
   int result_digit;
   int longer_num, shorter_num;
   bool same_length = false;

   string longer = "";
   ubigint result;

   //----------get the length of each number-------------
   if(ubig_value.size() > that.ubig_value.size())
   {
      longer_num = ubig_value.size();
      shorter_num = that.ubig_value.size();
      longer = "this";
      same_length = false;
   }
   if(ubig_value.size() < that.ubig_value.size())
   {
      longer_num = that.ubig_value.size();
      shorter_num = ubig_value.size();
      longer = "that";
      same_length = false;
   }
   if(ubig_value.size() == that.ubig_value.size())
   {
      longer_num = ubig_value.size();
      shorter_num = ubig_value.size();
      same_length = true;
   }
   //-----------------------------------------------------

   for(int index = 0; index < shorter_num; index++)
   {
      //get the digit of both numbers
      int left = ubig_value[index];
      int right = that.ubig_value[index];
      
      //if the sum of both digit and carry is greater than 9
      if(left + right + carry > 9)
      {
         //the carry is 1, and the digit is the reminder
         result_digit = (left + right + carry)% 10;
         carry = 1;
         result.ubig_value.push_back(result_digit);
      }
      else
      {
         //the carry is 0, the digit is sum
         result_digit = left + right + carry;
         carry = 0;
         result.ubig_value.push_back(result_digit);
      }
   }
   //if the numbers have different length
   if(not same_length)
   {
      //if the carry is 1
      if(carry == 1)
      {
         for (int index = shorter_num; index < longer_num; index++)
         {
            //if the longer number is that
            if(longer == "that")
            {
               //if the result is greater than 9
               if(that.ubig_value[index] + carry  > 9)
               {
                  //push bakc the result and set the carry to 1
                  result.ubig_value.push_back(
                     (that.ubig_value[index]+ carry)%10);
                  carry = 1;
               }
               else
               {
                  //if less than 9, set the carry to 0
                  result.ubig_value.push_back(
                     that.ubig_value[index] + carry );
                  carry = 0;
               }
            }
            //if the longer number is this
            if(longer == "this")
            {
               //if the result is greater than 9
               if(ubig_value[index] + carry > 9)
               {
                  //push back the result set the carry to 1
                  result.ubig_value.push_back(
                     (ubig_value[index]+ carry )%10);
                  carry = 1;
               }
               else
               {
                  //if the result is less than 9, set carry to 0
                  result.ubig_value.push_back(ubig_value[index]+carry);
                  carry = 0;
               }
            }
         }
      }
      //if the carry is 0
      else if(carry == 0)
      {
         for(int index = shorter_num; index < longer_num; index++)
         {
            if(longer == "that")
            {
               result.ubig_value.push_back(that.ubig_value[index]);
            }
            if(longer == "this")
            {
               result.ubig_value.push_back(ubig_value[index]);
            }
         }
      }
   }
   if(carry == 1)
   {
      result.ubig_value.push_back(carry);
   }
   return result;
}

ubigint ubigint::operator- (const ubigint& that) const {
   ubigint result;
   long unsigned int index = 0;
   int answer = 0;
   int this_value, that_value;
   int carryout = 0;
   if (*this < that) throw domain_error ("ubigint::operator-(a<b)");
   if ((this->ubig_value).size() == that.ubig_value.size()){
      if (ubig_value == that.ubig_value){
         answer = 0;
         result.ubig_value.push_back(answer);
      }
   }
   if ((ubig_value.size()) < (that.ubig_value.size())){
         throw domain_error ("ubigint::operator-(a<b)");
      }else{
      while (index < ubig_value.size()){
      this_value = ubig_value[index];
         if (index < that.ubig_value.size()){
            that_value = that.ubig_value[index];
         }else{
            that_value = 0;
         }
         answer = this_value - that_value - carryout;
         if (answer < 0){
            answer = answer + 10;
            carryout = 1;
         } else{
            carryout = 0;
         }
         result.ubig_value.push_back(answer);
         index++;
      }
      while ((result.ubig_value.size() > 0) && 
         (result.ubig_value[result.ubig_value.size()-1] == 0)){
         result.ubig_value.pop_back();
      }

   }
   return result;
}

ubigint ubigint::operator* (const ubigint& that) const {
   ubigint result;
   int length_thisvalue = ubig_value.size();
   int length_thatvalue = that.ubig_value.size();
   int answer = 0;
   int carryout = 0;
   int index,index_2;

   for(index=0; index<length_thisvalue+length_thatvalue; index++){
      result.ubig_value.push_back(0);
   }

   for(index=0; index<length_thatvalue; index++){
      for(index_2=0; index_2<length_thisvalue; index_2++){
         answer= result.ubig_value[index+index_2] + 
         ubig_value[index_2] * that.ubig_value[index];
         result.ubig_value[index+index_2] = answer;
         if(answer > 9){
            carryout = answer/10;
            result.ubig_value[index+index_2]=answer%10;
            result.ubig_value[index+index_2+1]=
            result.ubig_value[index+index_2+1]+carryout;
            answer=0;
            carryout=0;
         }
      }
   }

   while ((result.ubig_value.size() > 0) && 
      (result.ubig_value[result.ubig_value.size()-1] == 0)){
      result.ubig_value.pop_back();
   }
   return result;
}

void ubigint::multiply_by_2() {
   ubigvalue_t (this->ubig_value) = (this->ubig_value);
   ubigvalue_t another;
   int length_thisvalue= ubig_value.size();
   int carryout = 0;
   int index =0;
   int length_index,answer = 0;
   
   for (length_index=0; length_index<length_thisvalue;length_index++){
      another.push_back((this->ubig_value)[length_index]);
   }

   for(index = 0; index < length_thisvalue;){
      answer = another[index] + another[index] + carryout;
      if(answer > 9){
         answer = answer - 10;
         carryout = 1;
         (this->ubig_value)[index] = answer;
         index= index +1;
      }else{
         carryout=0;
         (this->ubig_value)[index] = answer;
         index= index +1;

      }
      
      if(index == length_thisvalue && carryout!=0){
         (this->ubig_value).push_back(1);
      }
      (this->ubig_value) = (this->ubig_value);
   }

}

void ubigint::divide_by_2() {
   ubigint result;
   int answer = 0, quo = 0, save = 0;

   for(int index = (this->ubig_value).size()-1; index>-1; index--){
      answer = ubig_value[index];

      if(save > 0){
         answer = answer + save * 10;
      }
      save = 0;
      quo = answer/2;
      save = answer%2;
      result.ubig_value.push_back(quo);
   }
   reverse(result.ubig_value.begin(),result.ubig_value.end());
   
   while ((result.ubig_value.size() > 0) && 
      (result.ubig_value[result.ubig_value.size()-1] == 0)){
   result.ubig_value.pop_back();
   }
   ubig_value = result.ubig_value;
}


struct quo_rem { ubigint quotient; ubigint remainder; };
quo_rem udivide (const ubigint& dividend, const ubigint& divisor_) {
   // NOTE: udivide is a non-member function.
   ubigint divisor {divisor_};
   ubigint zero {"0"};
   if (divisor == zero) throw domain_error ("udivide by zero");
   ubigint power_of_2 {"1"};
   ubigint quotient {"0"};
   ubigint remainder {dividend}; // left operand, dividend
   while (divisor < remainder) {
      divisor.multiply_by_2();
      power_of_2.multiply_by_2();
   }
   while (power_of_2 > zero) {
      if (divisor <= remainder) {
         remainder = remainder - divisor;

         quotient = quotient + power_of_2;
      }
      divisor.divide_by_2();
      power_of_2.divide_by_2();
   }
   return {.quotient = quotient, .remainder = remainder};
}

ubigint ubigint::operator/ (const ubigint& that) const {
   return udivide (*this, that).quotient;
}

ubigint ubigint::operator% (const ubigint& that) const {
   return udivide (*this, that).remainder;
}

bool ubigint::operator== (const ubigint& that) const {
   return !(*this < that || that < *this);
}

bool ubigint::operator< (const ubigint& that) const {
   int index = 0;
   int length_ubig_value = ubig_value.size();
   if (ubig_value.size() < that.ubig_value.size()){
      return true;
   }else if (ubig_value.size() == that.ubig_value.size()){
      while (index < length_ubig_value){
         if(ubig_value[length_ubig_value-1] < 
            that.ubig_value[length_ubig_value-1]){
            return true;
         }else if (ubig_value[length_ubig_value-1] == 
            that.ubig_value[length_ubig_value-1]){
            length_ubig_value = length_ubig_value - 1;
         }else if (ubig_value[length_ubig_value-1] > 
            that.ubig_value[length_ubig_value-1]){
            return false;
         }
      }
   }
   return false; 
}

ostream& operator<< (ostream& out, const ubigint& that) { 
  string result; 
   int limit = 69;
   int length_size = that.ubig_value.size();

   for (const char& c: that.ubig_value) {
      result.push_back(c + 48);
   }
   reverse(result.begin(), result.end());

   while (true) {
      if (limit < length_size)
      {
        result.insert(limit, "\\");
        result.insert(limit+1, "\n");
        limit = limit + 71;
        length_size = length_size + 2;
        continue;
      }else{
        break;
      }
   }

   return out << result; 
}

