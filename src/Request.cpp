#include "Request.h"

namespace Dragon {
void Request::flush(std::ofstream& out) {
    out << "# cmd \n\n\n";
    out << "# desc\n\n\n";
    out << "# request\n\n";
    out << method << "," << url.path << "\n\n";
    if (parameters.length() > 0) {
        out << "# parameters\n\n";
        out << "```json\n";
        out << parameters;
        out << "\n```\n";
    }
    out << "# response\n\n```json\n";
    out << response << "\n```\n";
    out << "=========================================================================\n";
}
}  // namespace Dragon