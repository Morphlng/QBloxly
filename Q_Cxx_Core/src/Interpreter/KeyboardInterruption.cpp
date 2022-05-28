#include "Interpreter/KeyboardInterruption.h"

namespace CXX {

KeyBoardInterruption::KeyBoardInterruption(const Position &start,
                                           const Position &end,
                                           std::string details)
    : Error(start, end, "Keyboard Interruption", std::move(details))
{}

KeyBoardInterruption::KeyBoardInterruption(Position *start, Position *end, std::string details)
    : Error(*start, *end, "Keyboard Interruption", std::move(details))
{}

} // namespace CXX
