// inc/receiveSentence.h
#pragma once
#ifndef RECEIVESENTENCE_H
#define RECEIVESENTENCE_H

// why: keep Arduino dependency in .cpp, header stays lightweight
void receiveSentence(int analogPin, int threshold);

#endif // RECEIVESENTENCE_H
