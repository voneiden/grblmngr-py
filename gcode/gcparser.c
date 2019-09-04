#include <stdlib.h>

typedef enum eFeedMode {
    RAPID=0,
    MOVE=1,
    CW_ARC=2,
    CCW_ARC=3
} t_feed_mode;

typedef enum eCoordinateMode {
    ABSOLUTE=90,
    RELATIVE=91
} t_coordinate_mode;

typedef enum eCoordinateUnit {
    INCHES=20,
    MILLIMETERS=21
} t_coordinate_unit;

typedef struct sProgramState {
    double x;
    double y;
    double z;
    t_feed_mode feed_mode;
    t_coordinate_mode coordinate_mode;
    t_coordinate_unit coordinate_unit;
} t_program_state;

typedef struct sBlock t_block;

typedef struct sProgram {
    t_block * start_block;
} t_program;


struct sBlock {
    t_block * block_before;
    t_program_state * state_before;
    t_block * block_after;
    t_program_state * state_after;
};

typedef struct sLine {

} line;

t_program * program = 0;



t_block * free_block(t_block * block, char free_state_before) {
    if (free_state_before) {
        free(block->state_before);
    }
    free(block->state_after);
    t_block * next_block = block->block_after;
    free(block);
    return next_block;
}

void free_program() {
    if (program != 0) {
        if (program->start_block != 0) {
            t_block * next_block = free_block(program->start_block, 1);
            while (next_block != 0) {
                next_block = free_block(next_block, 0);
            }
        }
        free(program);
        program = 0;
    }
}

int load(char * data) {
    free_program();

    return 1;
}

