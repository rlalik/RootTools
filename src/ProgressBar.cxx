#include "ProgressBar.h"

#include <iostream>

#define PR(x) std::cout << "++DEBUG: " << #x << " = |" << x << "| (" << __FILE__ << ", " << __LINE__ << ")\n";

using namespace RootTools;

ProgressBar::ProgressBar(long int cnt_limit, int point_width, int bar_width) :
	cnt_limit(cnt_limit), point_width(point_width), bar_width(bar_width), new_bar(true)
{
	line_prefix = "==> Processing data ";
}

void ProgressBar::setProgress(int current_location)
{
	current_location += 0;
}

long int ProgressBar::operator++()
{
}

void ProgressBar::render()
{
	if (new_bar)
	{
		std::cout << line_prefix << std::flush;
		new_bar = false;
	}

	for (long int i = cnt_previous; i < cnt_current; ++i)
	{
		if (i != 0  and ( (i+1) % point_width ) == 0)
			std::cout << "." << std::flush;

		if ((i != 0  and (i+1) % (point_width * bar_width) == 0) or
			(i == (cnt_limit-1)))
		{
			double num_percent = 100.0*(i+1)/cnt_limit;
			std::cout << " " << i+1 << " (" << num_percent << "%) " << "\n" << std::flush;
			new_bar = true;
		}
	}
}