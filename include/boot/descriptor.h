#ifndef DESCRIPTOR_H
#define DESCRIPTOR_H


/*
 *  * System Selectors
 *   */
#define NULL_SELECTOR       0x00

/********* x86 selectors *********/
#define CODE_SELECTOR       0x08
#define DATA_SELECTOR       0x10
#define USER_CODE_32_SELECTOR   0x18
#define USER_DATA_32_SELECTOR   0x20

/******* x86-64 selectors ********/
#define CODE_64_SELECTOR    0x28
#define STACK_64_SELECTOR   0x30
#define USER_CODE_64_SELECTOR   0x38
#define USER_DATA_64_SELECTOR   0x40

#define TSS_SELECTOR        0x48

#endif // DESCRIPTOR_H

