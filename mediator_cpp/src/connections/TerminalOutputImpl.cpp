#include "TerminalOutput.h"

void TerminalOutput::pushLine(const std::string& source, const std::string& text) {
    std::lock_guard<std::mutex> lock(mutex);
    buffer.push_back({std::chrono::system_clock::now(), source, text});
}

std::vector<OutputLine> TerminalOutput::drain(){
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<OutputLine> result(buffer.begin(), buffer.end());
    buffer.clear();
    return result;
}
