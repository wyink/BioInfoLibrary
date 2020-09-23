#pragma once
#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <set>
#include <map>

namespace fs = std::filesystem;


class Analysis
{

};


class Fmeasure {
private:
	const fs::path& indir;
	const fs::path& outfile;
public:

	inline Fmeasure(const fs::path& indir, const fs::path& outfile):
		indir(indir),outfile(outfile) {}

	void run();
};