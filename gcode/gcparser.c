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

t_program program = {0};

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


#define MAX_LINE_LENGTH 256
long parse_long(char ** bytes_pointer, long * counter, long * result) {
    char * bytes = *bytes_pointer;
    char * bytes_end = NULL;
    *result = strtol(bytes, &bytes_end, 10);
    // TODO check for error
    long length = bytes_end - bytes;
    *counter += length;
    bytes += length;
    return 0;

}
long parse_double(char ** bytes_pointer, long * counter, double * result) {
    char * bytes = *bytes_pointer;
    char * bytes_end = NULL;
    *result = strtod(bytes, &bytes_end);
    // TODO check for error
    long length = &bytes_end - &bytes;
    *counter += length;
    bytes += length;
    return 0;
}

#define CHECK_ERROR \
    if (error) { \
        return 1; \
    } \
    state_changed = 1;

long parse_block(char ** bytes_pointer) { //t_program_state state
    long state_changed = 0;
    char * bytes = *bytes_pointer;
    char byte;
    long error;
    long l_value;
    double d_value;

    for (long i=0; i < MAX_LINE_LENGTH; i++) {
        byte = *bytes++;
        switch (byte) {
            case '\r':
            case '\n':
            case '\0':
                return 0;
            case 'G':
            case 'g':
                // TODO doesn't seem to update!
                error = parse_long(&bytes, &i, &l_value);
                CHECK_ERROR
                break;
            case 'X':
            case 'x':
                error = parse_double(&bytes, &i, &d_value);
                CHECK_ERROR
                break;
            case 'Y':
            case 'y':
                error = parse_double(&bytes, &i, &d_value);
                CHECK_ERROR
                break;
            case 'Z':
            case 'z':
                error = parse_double(&bytes, &i, &d_value);
                CHECK_ERROR
                break;
            case 'F':
                error = parse_double(&bytes, &i, &d_value);
                CHECK_ERROR
                break;

            default:
                break;
        }
    }

    //bytes++;
    return 1;
}

int load(char * bytes) {
    free_program();
    long error;
    // TODO this pointer is not udpating
    // TODO this pointer will also overshoot so the equality chec kis not OK
    while (*bytes != '\0') {
        error = parse_block(&bytes);
    }

    return 1;
}


int main() {
    load("G10");
    return 1;

}