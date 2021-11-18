/**
 * Created by Martin on 18.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include "state.h"

State::State() {
    this->reset();
}

void State::reset() {
    this->terminated = false;
}