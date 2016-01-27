#include "ProgressBar.h"

#include <iostream>

#define PR(x) std::cout << "++DEBUG: " << #x << " = |" << x << "| (" << __FILE__ << ", " << __LINE__ << ")\n";

using namespace RootTools;

ProgressBar::ProgressBar(long int cnt_limit, int point_width, int bar_width) :
	cnt_current(0), cnt_previous(0), cnt_limit(cnt_limit),
	point_width(point_width), bar_width(bar_width),
	new_bar(true), new_bar_line(true),
	bar_p('.'), alarm_p('!')
{
	line_prefix = "==> Processing data ";
}

void RootTools::ProgressBar::close()
{
	std::cout << std::endl;
}

void ProgressBar::setProgress(int current_location)
{
	cnt_current = current_location;
	render();
}

ProgressBar & ProgressBar::operator++()
{
	++cnt_current;
	render();

	return *this;
}

ProgressBar ProgressBar::operator++(int)
{
	ProgressBar pb(*this);
	++(*this);

	return pb;
}

void ProgressBar::render()
{
	for (long int i = cnt_previous; i < cnt_current; ++i)
	{
		if (new_bar or new_bar_line)
		{
			std::cout << line_prefix << std::flush;
			new_bar = false;
			new_bar_line = false;
		}

		if (i != 0  and ( (i+1) % point_width ) == 0)
		{
			if (i < cnt_limit)
				std::cout << bar_p << std::flush;
			else
				std::cout << alarm_p << std::flush;
		}

		if ((i != 0  and (i+1) % (point_width * bar_width) == 0) or
			(i == (cnt_limit-1)))
		{
			double num_percent = 100.0*(i+1)/cnt_limit;
			std::cout << " " << i+1 << " (" << num_percent << "%) " << "\n" << std::flush;

			new_bar_line = true;
		}
	}

	cnt_previous = cnt_current;
}