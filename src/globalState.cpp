#include "globalState.hpp"

globalState::globalState(logger& log, ValueMapper<ino_t, ino_t> inodeMap,
                         ValueMapper<ino_t, time_t> mtimeMap):
  inodeMap{ inodeMap },
  mtimeMap{ mtimeMap },
  log(log){
  }