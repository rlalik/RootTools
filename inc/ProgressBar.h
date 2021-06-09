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

    void close();

    void setProgress(int current_location);
    ProgressBar& operator++();
    ProgressBar operator++(int);

    // set/get line prefix
    inline void setLinePrefix(const std::string& prefix) { line_prefix = prefix; }
    inline std::string getLinePrefix() const { return line_prefix; }

    inline void setBarCharacter(char p) { bar_p = p; }
    inline void setAlarmCharacter(char p) { alarm_p = p; }

private:
    void render();

protected:
    long int cnt_current;
    long int cnt_previous;
    long int cnt_limit;
    int point_width;
    int bar_width;
    bool new_bar;
    bool new_bar_line;

    std::string line_prefix;
    char bar_p;
    char alarm_p;
};

}; // namespace RootTools

#endif /* PROGRESSBAR_H */
