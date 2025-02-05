/**
 * @file StateMachine.h
 * @author Anders Boghammar
 * @brief Sensor node statemachine
 * @version 0.1
 * @date 2025-01-20
 * 
 * @copyright Copyright (c) boghammar.com 2025
 *
 */

#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "wstring.h"

enum class State {
    SAMPLEDATA,
    SENDDATA,
    RECEIVEMESSAGE,
    SLEEP
};

class StateMachine {
public:
  StateMachine()  : currentState(State::SAMPLEDATA) {}
  ~StateMachine();

  void sampleDataDone() {currentState = State::SENDDATA;}
  void sendDataDone() {currentState = State::RECEIVEMESSAGE;}
  void sendNoDataDone() {currentState = State::SLEEP;}
  void receiveMessageDone() {currentState = State::SLEEP;}

  String state() {switch (currentState) {
    case State::SAMPLEDATA: return "SAMPLE";
    case State::SENDDATA: return "SEND";
    case State::RECEIVEMESSAGE: return "RECEIVE";
    case State::SLEEP: return "SLEEP";
    default: return "Unknown state";
    }}

  State currentState;

private:
};

#endif