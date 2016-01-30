#include <stdio.h>
#include <assert.h>
#include "turing.h"

#define TAPE_SIZE 1024
physical_cell_t physical_tape[TAPE_SIZE];
virtual_symbol_t virtual_tape[TAPE_SIZE];

const rule_t rules[] = {
	{.state_in = BEGIN, .symbol_in = '*', .symbol_out = 'b', .direction = '*', .state_out = A}
};

virtual_symbol_t *virtual_current = virtual_tape; // equals &(virtual_tape[0])
physical_cell_t *physical_current = physical_tape;
state_t state = BEGIN;

// Moves on the physical tape
void physical_move(direction_t d) {
	/*
	This is a virtual implementation.
	On a real-life WORM Turing machine, this would control motors to move the physical tape.
	*/
	switch (d) {
		case RIGHT:
			printf("Moving right\n");
			physical_current++;
			break;
		case LEFT:
			printf("Moving left\n");
			physical_current--;
			break;
	}
}

void physical_goto(physical_cell_t *destination) {
	while (destination > physical_current)
		physical_move(RIGHT);
	while (destination < physical_current)
		physical_move(LEFT);
	assert(destination == physical_current);
}

// Moves on the virtual tape
void virtualMove(direction_t d) {
	switch (d) {
		case RIGHT:
			virtual_current++;
			break;
		case LEFT:
			virtual_current--;
			break;
	}
}

symbol_t physical_read_current() {
	/*
	Reads a symbol from the current position on the physical tape.
	This is a virtual implementation.
	In a real-life WORMTuring machine, this would use sensors.
	*/
	return physical_current->symbol;
}

// Get the first fresh cell
physical_cell_t* get_write_addr() {
	int i;

	for (i = 0; i < TAPE_SIZE; i++) {
		if (!physical_tape[i].written)
			return physical_tape + i;
	}
	printf("Ran out of memory!\n");

	return 0;
}

int rules_num;

void init() {
	int i;

	/*
	In the beginning, the virtual tape maps directly to the virtual tape,
	because no cells are occupied.
	*/
	for (i = 0; i < TAPE_SIZE; i++)
		virtual_tape[i].read_addr = virtual_tape[i].write_addr = physical_tape + i;

	rules_num = sizeof(rules)/sizeof(rules[0]);
}

symbol_t read() {
	physical_goto(virtual_current->read_addr);
	return physical_read_current();
}

// Writes to the current cell on the physical tape
void physical_write_current(symbol_t value) {
	assert(physical_current->written == 0); // Can only write to fresh cells
	/*
	This is a virtual implementation.
	In a real-life WORMTuring machine, this would write to a physical tape.
	*/
	physical_current->symbol = value;
	physical_current->written = 1;
}

void write(symbol_t value) {
	physical_goto(virtual_current->write_addr);
	physical_write_current(value);
	// The read pointer now points to the cell we just wrote
	virtual_current->read_addr = virtual_current->write_addr;
	// The write pointer now points to a fresh cell
	virtual_current->write_addr = get_write_addr();
}

int main() {
	int i; // Counter variable - removes a gcc warning

	init();

	for (i = 0; i < rules_num; i++) {
		#define rule (rules[0])

		// Discard state mismatches
		if (rule.state_in != ANY && rule.state_in != state)
			continue;
		// Discard symbol mismatches
		if (rule.symbol_in != '*' && rule.symbol_in != read())
			continue;

		// State change
		if (rule.state_out != ANY)
			state = rule.state_out;
		// Symbol change
		if (rule.symbol_out != '*') {
			write(rule.symbol_out);
		}

		// Tape movement
		switch (rule.direction) {
			case 'r':
				virtualMove(RIGHT);
				break;
			case 'l':
				virtualMove(LEFT);
				break;
			case '*':
				// Do nothing
				break;
			default:
				printf("Unknown direction!\n");
//				printf("%c", rule.direction);
				return 1;
		}
		break;
	}

	printf("Final state: %i\n", state);
	printf("Final position on the (virtual) tape: %i\n", virtual_current - virtual_tape);
	printf("Final symbol on the tape: %i (%c)\n", read(), read());
	printf("-----\nStatistics\n");
	printf("Physical tape length: %i\n", get_write_addr() - physical_tape);
	return 0;
}