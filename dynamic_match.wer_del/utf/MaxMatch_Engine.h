#pragma once

#include "Term.h"
#include "MaxMatch.h"
#include "StringUtil.h"

class MaxMatch_Engine
{
	MaxMatch maxMatch;

public:
	MaxMatch_Engine(void);
	~MaxMatch_Engine(void);

	vector<Term> wordSegmentor(string sent);

	string handleSent(string sent);

	void handleFile(string filePath, string outputPath);

	void handleDir(string inputPath, string outputPath);

};

