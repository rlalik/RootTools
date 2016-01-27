#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <string>

namespace RootTools
{

class ProgressBar
{
public:
	ProgressBar(long int limit, int point_width = 500, int bar_width = 20);
	virtual ~ProgressBar() {}

	void setProgress(int current_location);
	ProgressBar & operator++();
	ProgressBar operator++(int);

private:
	void render();

protected:
	long int cnt_current;
	long int cnt_previous;
	long int cnt_limit;
	int point_width;
	int bar_width;
	bool new_bar;

	std::string line_prefix;
};

};

#endif /* PROGRESSBAR_H */
