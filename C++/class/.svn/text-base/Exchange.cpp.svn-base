/*
 * Exchange.cpp
 *
 *  Created on: Feb 5, 2013
 *      Author: sharmaj
 *
 *      Monostate design pattern to share data between classes.
 */
#include <iostream>

using namespace std;

class Admin;

class Exchange
{
private:
static double Euro; // Euros per $1 U.S.
static double GBP; // British pounds per $1 U.S.
static double Yen;  // Yens per $1 U.S.
//...
//
public:
friend class Admin;
 double Euro_to_USD(double euros) const { return euros/Euro;}
 double US_to_Euro(double us_dollars) const { return us_dollars*Euro;}
 double GBP_to_USD(double pounds) const { return pounds/GBP;}
 double USD_to_GBP(double us_dollars) const { return us_dollars*GBP;}
//
};

double Exchange::Euro=0.8630; // Euros per $1 U.S.
double Exchange::GBP=0.6017; // British Pounds per $1 U.S.
double Exchange::Yen= 119.34; // Yens per $1 U.S.

class Admin
{
public:
 Admin(const string &password) {} //authorize
  void set_EURO(double euros) const {Exchange::Euro = euros;}
  void set_GBP(double pounds) const {Exchange::GBP = pounds;}
//
};

int main(){
 Exchange xcg;  //cosmetics dept.'s private object
 double pounds=xcg.USD_to_GBP(99.95);//convert $99.95 to GBP
 cout<<"equivalent amount in British pounds: " << pounds << endl;

 Admin admin("mypassword");
 admin.set_GBP(0.62);
 Exchange rates;
 pounds=rates.USD_to_GBP(99.95);
 cout<<"equivalent amount in British pounds: " << pounds << endl;

 return 0;
}
