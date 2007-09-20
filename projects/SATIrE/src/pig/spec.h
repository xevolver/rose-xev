// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: spec.h,v 1.3 2007-09-20 09:25:32 adrian Exp $

#ifndef H_SPEC
#define H_SPEC

#include <string>

#include "GrammarIr.h"

enum types { T_ID, T_STR, T_NUM };

struct isn
{
    enum types type;
    unsigned long line, col;
    char *id, *str;
    unsigned long num;
};

std::string splice(std::string s);
std::string macroname(std::string rulename, unsigned long ruleno);

GGrammar *synparse(void);

#endif
