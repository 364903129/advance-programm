// $Id: bigint.cpp,v 1.2 2020-01-06 13:39:55-08 - - $
//haofan wang hwang108@ucsc.edu
//Qihang Huang qhuang24@ucsc.edu

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "bigint.h"
#include "debug.h"
#include "relops.h"

static const bigint zero (0);

bigint::bigint (long that): uvalue (that), is_negative (that < 0) {
   DEBUGF ('~', this << " -> " << uvalue)
}

bigint::bigint (const ubigint& uvalue_, bool is_negative_):
                uvalue(uvalue_), is_negative(is_negative_) {
}

bigint::bigint (const string& that) {
   is_negative = that.size() > 0 and that[0] == '_';
   uvalue = ubigint (that.substr (is_negative ? 1 : 0));
}

bigint bigint::operator+ () const {
   return *this;
}

bigint bigint::operator- () const {
   return {uvalue, not is_negative};
}

bigint bigint::operator+ (const bigint& that) const {
   bigint result;
   const ubigint first = this->uvalue;
   const ubigint second = that.uvalue;
   //if both sign are the same,add both number,
   //the sign is same as first number
   if((this->is_negative == false && that.is_negative == false) ||
    (this->is_negative == true && that.is_negative == true))
   {
      result.uvalue = first + second;
      result.is_negative = this->is_negative;
   }
   //if both sign are not the same
   else
   {
      //if first number is greater,value is first - second,
      //sign is same as first
      if(first > second)
      {
         result.uvalue = first - second;
         result.is_negative = this->is_negative;
      }
      //if first number is less,value is second - first,
      //sign is same as second
      if(first < second)
      {
         result.uvalue = second - first;
         result.is_negative = that.is_negative;
      }
      //if first equal second, value is 0, sign is positive
      if(first == second)
      {
         result.uvalue = first - second;
         result.is_negative = false;
      }
   }
   return result;
}

bigint bigint::operator- (const bigint& that) const {
   bigint result;
   const ubigint first = this->uvalue;
   const ubigint second = that.uvalue;
   //if both sign are the same
   if((this->is_negative == false && that.is_negative == false) ||
    (this->is_negative == true && that.is_negative == true))
   {
      //if first is greater than second, uvalue is first - second,
      //the sign is same as first number
      if(first > second)
      {
         result.uvalue = first - second;
         result.is_negative = this->is_negative;
      }
      //if first is less than second, uvalue is second - first,
      //the sign is opposite of second number
      if(first < second)
      {
         result.uvalue = second - first;
         result.is_negative = not this->is_negative;
      }
      //if first = second, uvalue is 0, sign is positive
      if(first == second)
      {
         result.uvalue = first - second;
         result.is_negative = false;
      }
   }
   //if both sign are different, value is first + second,
   //sign is same as first number
   else
   {
      result.uvalue = first + second;
      result.is_negative = this->is_negative;
   }
   return result;
}


bigint bigint::operator* (const bigint& that) const {
   bigint result;
   const ubigint first = this->uvalue;
   const ubigint second = that.uvalue;
   //the value of the result is first * second,
   //no matter with the sign.
   result.uvalue = first * second;
   //if signs are the same
   if((this->is_negative == false && that.is_negative == false) ||
    (this->is_negative == true && that.is_negative == true))
   {
      //the sign for result is positive
      result.is_negative = false;
   }
   //if signs are different
   else
   {
      //the sign for result is negative
      result.is_negative = true;
   }
   return result;
}

bigint bigint::operator/ (const bigint& that) const {
   bigint result;
   const ubigint first = this->uvalue;
   const ubigint second = that.uvalue;
   //check if the divisor is zero or not
   if(that == zero)
   {
      cout<<"error: can not divided by 0"<<endl;
      exit (EXIT_FAILURE);
   }
   result.uvalue = first / second;
   //if result is zero, the sign is positive
   if(result == zero)
   {
      result.is_negative = false;
   }
   //if signs are the same, the result sign is positive
   else if ((this->is_negative == false && that.is_negative == false) ||
    (this->is_negative == true && that.is_negative == true))
   {
      result.is_negative = false;
   }
   //if signs are different, the result sign is negative
   else
   {
      result.is_negative = true;
   }
   return result;
}

bigint bigint::operator% (const bigint& that) const {
   bigint result = uvalue % that.uvalue;
   return result;
}

bool bigint::operator== (const bigint& that) const {
   return is_negative == that.is_negative and uvalue == that.uvalue;
}

bool bigint::operator< (const bigint& that) const {
   if (is_negative != that.is_negative) return is_negative;
   return is_negative ? uvalue > that.uvalue
                      : uvalue < that.uvalue;
}

ostream& operator<< (ostream& out, const bigint& that) {
   return out <<(that.is_negative ? "-" : "")
              << that.uvalue;
}

