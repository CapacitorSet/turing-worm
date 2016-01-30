typedef enum {
	BEGIN, // The initial state
	ANY, // Match any state/do not change the state
	A,
	B
} state_t;

typedef char symbol_t;

typedef struct {
	symbol_t symbol;
	_Bool written; // Is the cell not free?
} physical_cell_t;

typedef struct {
	physical_cell_t *read_addr; // Points to a readable cell
} virtual_symbol_t;

typedef struct {
	state_t state_in;
	symbol_t symbol_in;
	symbol_t symbol_out;
	char direction; // 'r', 'l' or '*'. '*' doesn't move the tape.
	state_t state_out;
} rule_t;

typedef enum{LEFT, RIGHT} direction_t;