#ifndef H_TERMINAL_OUTPUT
#define H_TERMINAL_OUTPUT

#include <string>
#include <vector>
#include <deque>
#include <mutex>
#include <chrono>

struct OutputLine {
    std::chrono::system_clock::time_point timestamp;
    std::string source; // e.g. "ssh", "mavlink", "mediator"
    std::string text;
};

// Multiple producers, only one consumer
class TerminalOutput {
private:
    std::mutex mutex;
    std::deque<OutputLine> buffer;

public:
    void pushLine(const std::string& source, const std::string& text);
    std::vector<OutputLine> drain(); // returns + clears everything buffered
};

#endif
