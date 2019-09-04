#include <Python.h>
#include <string.h>

#define ALLOCATE 1000
#define BUFFER_SIZE 20


static char module_docstring[] =
    "This module provides an interface for doing vector subtraction.";
static char parse_docstring[] =
    "Perform vector subtraction.";


typedef enum MOTION_MODE {RAPID = 1, FEED, CW_ARC, CCW_ARC} t_motion_mode;
typedef enum COORDINATE_MODE {ABSOLUTE = 1, RELATIVE} t_coordinate_mode;
typedef enum UNIT_MODE {METRIC = 1, IMPERIAL} t_unit_mode;

typedef struct s_machine_state {
    t_motion_mode motion_mode;
    double x;
    double y;
    double z;
} t_machine_state;

struct s_execution_buffer {
    t_motion_mode motion_mode;
    t_coordinate_mode coordinate_mode;
    t_unit_mode unit_mode;

    char mask; // mask to determine which of x,y,z is enabled for execution
    double x;
    double y;
    double z;
} execution_buffer = {0};

static void reset_execution_buffer() {
    execution_buffer.motion_mode = 0;
    execution_buffer.coordinate_mode = 0;
    execution_buffer.unit_mode = 0;
    execution_buffer.mask = 0;
}

struct s_modal_buffer {
    void *target;
    enum { is_double, is_int } type;
    char buffer[BUFFER_SIZE];
    int length;
} modal_buffer = {0};


static void reset_modal_buffer() {
    modal_buffer.target = NULL;
    modal_buffer.buffer[0] = '\0';
    modal_buffer.length = 0;
}

static char apply_modal_buffer() {
    if (modal_buffer.type == is_double && modal_buffer.target) {
        if (modal_buffer.length == 0) {
            PyErr_SetString(PyExc_TypeError, "Encountered incomplete word (value missing)");
            return 0;
        }
        modal_buffer.buffer[modal_buffer.length] = '\0';
        int modal;
        switch (modal_buffer.type) {
            case is_double:
                *(double*) modal_buffer.target = atof(modal_buffer.buffer);
                break;
            case is_int:
                modal = atoi(modal_buffer.buffer);
                switch (modal) {
                    case 0:
                        execution_buffer.motion_mode = RAPID;
                        break;
                    case 1:
                        execution_buffer.motion_mode = FEED;
                        break;
                    case 2:
                        execution_buffer.motion_mode = CW_ARC;
                        break;
                    case 3:
                        execution_buffer.motion_mode = CCW_ARC;
                        break;
                    case 20:
                        execution_buffer.unit_mode = IMPERIAL;
                        break;
                    case 21:
                        execution_buffer.unit_mode = METRIC;
                        break;
                    case 90:
                        execution_buffer.coordinate_mode = ABSOLUTE;
                        break;
                    case 91:
                        execution_buffer.coordinate_mode = RELATIVE;
                        break;
                    default:
                        break;
                }
                break;

            default:
                PyErr_SetString(PyExc_TypeError, "Instruction type was not defined");
                return 0;
        }
    }
    reset_modal_buffer();
    return 1;
}


static PyObject *parse_entrypoint(PyObject *self, PyObject *py_bytes)
{
    if (!PyBytes_Check(py_bytes)) {
        PyErr_SetString(PyExc_TypeError, "argument must be bytes");
        return NULL;
    }

    char *bytes = PyBytes_AS_STRING(py_bytes);

    Py_ssize_t bytes_size = PyBytes_Size(py_bytes);

    reset_modal_buffer();
    char comment = 0;

    for (Py_ssize_t i = 0; i < bytes_size; i++) {
        char b = bytes[i];


        if (b == '\n') {
            // exec buffer
            char success = apply_modal_buffer();
            if (!success) {
                return NULL;
            }
            continue;
        }
        if (comment == 1) {
            if (b == ')') {
                comment = 0;
            }
            continue;
        }

        switch (b) {
            case '(':
                comment = 1;
                continue;
            case 'X':
            case 'x':
                apply_modal_buffer();
                modal_buffer.target = &execution_buffer.x;
                modal_buffer.type = is_double;
                continue;
            case 'Y':
            case 'y':
                apply_modal_buffer();
                modal_buffer.target = &execution_buffer.y;
                modal_buffer.type = is_double;
                continue;
            case 'Z':
            case 'z':
                apply_modal_buffer();
                modal_buffer.target = &execution_buffer.z;
                modal_buffer.type = is_double;
                continue;

            case 'G':
            case 'g':


            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '.':
                if (modal_buffer.target == NULL) {
                    PyErr_SetString(PyExc_TypeError, "Encountered numbers before a modal was set");
                    return NULL;
                }
                if (modal_buffer.length >= BUFFER_SIZE - 1) {
                    PyErr_SetString(PyExc_TypeError, "Huge numbers? Ran out of buffer");
                    return NULL;
                }

                // Skip decimals when we're processing int types
                if (modal_buffer.type == is_int && b == '.') {
                    continue;
                }
                modal_buffer.buffer[modal_buffer.length] = b;
                modal_buffer.length++;
                continue;
        }
    }

    char success = apply_modal_buffer();
    if (!success) {
        return NULL;
    }

    PyObject *ret = Py_BuildValue("d", execution_buffer.x);
    return ret;
}


static PyMethodDef module_methods[] = {
    {"parse", parse_entrypoint, METH_O, parse_docstring},
    {NULL, NULL, 0, NULL}
};



static struct PyModuleDef module_def = {
    PyModuleDef_HEAD_INIT,
    "gcparser",
    module_docstring,
    -1,
    module_methods,
};

PyMODINIT_FUNC PyInit_gcparser(void)
{
    return PyModule_Create(&module_def);
}
