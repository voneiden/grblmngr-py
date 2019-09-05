#include <stdlib.h>
#include <ctype.h>

typedef enum eFeedMode {
    RAPID = 0,
    MOVE = 1,
    CW_ARC = 2,
    CCW_ARC = 3
} t_feed_mode;

typedef enum eCoordinateMode {
    ABSOLUTE = 90,
    RELATIVE = 91
} t_coordinate_mode;

typedef enum eCoordinateUnit {
    INCHES = 20,
    MILLIMETERS = 21
} t_coordinate_unit;

typedef struct sProgramState {
    double x;
    double y;
    double z;
    t_feed_mode feed_mode;
    t_coordinate_mode coordinate_mode;
    t_coordinate_unit coordinate_unit;
} t_program_state;


typedef struct sBlock {
    char *bytes;
} t_block;

typedef struct sSegment t_segment;
struct sSegment {
    t_program_state *state_before;
    t_program_state *state_after;
    t_block *start_block;
};


typedef struct sProgram {
    long n_blocks;
    t_block *blocks;
    long n_segments;
    t_segment *segments;
} t_program;

t_program program = {NULL};

/*
t_segment * free_block(t_segment * block, char free_state_before) {
    if (free_state_before) {
        free(block->state_before);
    }
    free(block->state_after);
    t_segment * next_block = block->segment_after;
    free(block);
    return next_block;
}
*/
void free_block(t_block *block) {
    free(block->bytes);
    free(block);
}

void free_segment(t_segment *segment, long skip_state_before) {
    if (skip_state_before) {
        free(segment->state_before);
    }
    free(segment->state_after);
}
t_program_state * create_program_state() {
    t_program_state * state =  malloc(sizeof(t_program_state));
    state->coordinate_mode=-1;
    state->coordinate_unit=-1;
    state->feed_mode=-1;
    return state;
}

t_segment * create_segment(t_segment * segment_before) {
    t_segment * segment = malloc(sizeof(t_segment));
    if (segment_before) {
        segment->state_before = segment_before->state_after;
    } else {
        segment->state_before = create_program_state();
    }
    segment->state_after = create_program_state();
    return segment;
}


void free_program() {
    for (long nb = 0; nb < program.n_blocks; nb++) {
        free_block(&program.blocks[nb]);
    }
    for (long ns = 0; ns < program.n_segments; ns++) {
        free_segment(&program.segments[ns], ns);
    }
}



void parse_block(char * bytes) {
    // Maybe goto sucks here..
    char byte;
    char n_buffer = 0;
    char buffer[256];

    read_next_word:
    byte = *bytes;
    bytes++;
    switch (byte) {
        case '\r':
        case '\n':
        case '\0':
            goto done;
        case 'G':
        case 'g':
            goto read_general_function;
        default:
            goto read_next_word;
    }

    read_general_function:
    //bytes++;
    byte = *bytes++;
    if (byte == '.') {

    }

    if (byte == '.' || isdigit(byte)) {
        buffer[n_buffer] = byte;
        n_buffer++;
    } else {
        buffer[n_buffer] = '\0';
        n_buffer = 0;
    }
    done:
    return;
}

int load(char * bytes) {
    free_program();


    return 1;
}

