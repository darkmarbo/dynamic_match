#pragma once

#include "Dictionary.h"
#include "Term.h"

class MaxMatch
{
	Dictionary dictionary;

public:
	MaxMatch(void);
	~MaxMatch(void);

	vector<Term> ForwardMaximumMatching(const string& str);
	vector<Term> BackwardMaximumMatching(const string& str);
	vector<Term> Forward_BackwardMaximumMatching(const string& str);

};

