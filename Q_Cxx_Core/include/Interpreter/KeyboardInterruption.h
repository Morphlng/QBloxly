#pragma once

#include "Common/Error.h"

namespace CXX {

class KeyBoardInterruption : public Error
{
public:
    KeyBoardInterruption(const Position &start, const Position &end, std::string details);
    KeyBoardInterruption(Position *start, Position *end, std::string details);
};

} // namespace CXX
