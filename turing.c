#include <stdio.h>
#include "turing.h"

#define TAPE_SIZE 1024
physical_cell_t physical_tape[TAPE_SIZE];
virtual_symbol_t virtual_tape[TAPE_SIZE];

const rule_t rules[] = {
	{.state_in = BEGIN, .symbol_in = '*', .symbol_out = 'b', .direction = '*', .state_out = A}
};

virtual_symbol_t *current = virtual_tape; // equals &(virtual_tape[0])
state_t state = BEGIN;

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

int main() {
	int i; // Counter variable - removes a gcc warning

	// In the beginning, the virtual tape maps directly to the virtual tape, because no cells are occupied
	for (i = 0; i < TAPE_SIZE; i++)
		virtual_tape[i].read_addr = virtual_tape[i].write_addr = physical_tape + i;

	const int rules_num = sizeof(rules)/sizeof(rules[0]);

	for (i = 0; i < rules_num; i++) {
		#define rule (rules[0])

		// Discard state mismatches
		if (rule.state_in != ANY && rule.state_in != state)
			continue;
		// Discard symbol mismatches
		if (rule.symbol_in != '*' && rule.symbol_in != current->read_addr->symbol)
			continue;

		// State change
		if (rule.state_out != ANY)
			state = rule.state_out;
		// Symbol change
		if (rule.symbol_out != '*') {
			// Write
			current->write_addr->symbol = rule.symbol_out;
			// Flag as written
			current->write_addr->written = 1;
			// The read pointer now points to the cell we just wrote
			current->read_addr = current->write_addr;
			// The write pointer now points to a fresh cell
			current->write_addr = get_write_addr();
		}

		// Tape movement
		switch (rule.direction) {
			case 'r':
				current++;
				break;
			case 'l':
				current--;
				break;
			case '*':
				// Do nothing
				break;
			default:
				printf("Unknown direction!\n");
//				printf("%c", rule.direction);
				return 1;
		}
	}

	printf("Final state: %i\n", state);
	printf("Final position on the tape: %i\n", current - virtual_tape);
	printf("Final symbol on the tape: %i (%c)\n", current->read_addr->symbol, current->read_addr->symbol);

	return 0;
}