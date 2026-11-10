#pragma once
#include "String.hpp"

String operator+(const String& s, wchar_t c);
String operator+(const String& s, int n);

class fPoint;
String& appendFPoint(String& out, const fPoint* p);

class Strings;
String& appendStrings(String& out, const Strings& list);
