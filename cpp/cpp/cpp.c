
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "structmember.h"


typedef struct {
    PyObject_HEAD    
    int graph_vertices;
    int graph_adjacencyMatrix[16];
} AdjacencyMatrix;

void PrintMatrix(int bytes[16]) {
    for (int i = 0; i < 16; i++) {
        printf("%d\n", bytes[i]);
    }
}
void AddEdge(AdjacencyMatrix* self, int i, int j) {
    if (i < j) {
        int temp = i;
        i = j;
        j = temp;
    }

    self->graph_adjacencyMatrix[i] |= (1 << j);
}
static void
Custom_dealloc(AdjacencyMatrix* self)
{    
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject*
Custom_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{

    AdjacencyMatrix* self;
    self = (AdjacencyMatrix*)type->tp_alloc(type, 0);
    if (self != NULL) {
        self->graph_vertices = 0;
        for (int i = 0; i < 16; i++) {
            self->graph_adjacencyMatrix[i] = 0;
        }
    }
    return (PyObject*)self;
}

static int
Custom_init(AdjacencyMatrix* self, PyObject* args, PyObject* kwds)
{
    static char* kwlist[] = { "text", NULL };    

    char* bytes = "?";            

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &bytes)) 
        return -1;            
    int verticesNumber = bytes[0]-63;    

    int k = 0,i = 1;
    int c = 0;
    for (int v = 0; v < verticesNumber; v++) {
        self->graph_vertices |= 1 << v;
    }        
    
    for (int v = 1; v < verticesNumber; v++) {        
        for (int u = 0; u < v; u++) {
            if (k == 0) {
                c = bytes[i] - 63;                
                i++;
                k = 6;
            }
            k -= 1;
            if ((c & (1 << k)) != 0) {                                
                self->graph_adjacencyMatrix[v] |= 1 << u;                                   
            }
        }
    }    
    return 0;
}



static PyObject*
WheelInit(PyObject* Py_UNUSED(ignored),
    PyTypeObject* defining_class,
    PyObject* const* args,
    Py_ssize_t nargs,
    PyObject* kwnames)
{
 
    AdjacencyMatrix* self;
    self = (AdjacencyMatrix*)defining_class->tp_alloc(defining_class, 0);
    if (self != NULL) {
        self->graph_vertices = 0;
        for (int i = 0; i < 16; i++) {
            self->graph_adjacencyMatrix[i] = 0;
        }
    }
   
    static char* kwlist[] = { "number_of_vertices", NULL };    
    int number_of_vertices = 0;    

    
    if (nargs < 1) {
        printf("ERROR\n");
        Py_RETURN_NONE;
    }
    
    number_of_vertices = PyLong_AsLong(args[0]);

    for (int i = 0; i < number_of_vertices; i++) {
        self->graph_vertices |= 1 << i; //add vertex
    }
    for (int j = 0; j < number_of_vertices; j++) {
        //self->graph_adjacencyMatrix[number_of_vertices - 1] |= 1 << j; //to center
        AddEdge(self, j, number_of_vertices - 1);//to center        
        AddEdge(self, j, (j + 1) % (number_of_vertices-1));//outer edge
    }    
    
    return self;
}

static PyMemberDef Custom_members[] = {
    {"graph_vertices", T_INT, offsetof(AdjacencyMatrix, graph_vertices), 0,"custom aa"},
    {"graph_adjacencyMatrix", T_OBJECT_EX, offsetof(AdjacencyMatrix, graph_adjacencyMatrix), 0,"custom matrix"},
    
    {NULL}  /* Sentinel */
};

#pragma region METHODS

static PyObject*
print_matrix(AdjacencyMatrix* self, PyObject* Py_UNUSED(ignored))
{    

    
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            if ((self->graph_adjacencyMatrix[i] & (1 << j)) != 0) {
                printf("1");                
            }
            else {
                printf("0");
            }            
        }
        printf("\n");
    }
    Py_RETURN_NONE;
}


static PyObject*
number_of_vertices(AdjacencyMatrix* self, PyObject* Py_UNUSED(ignored))
{
    Py_ssize_t count = 0;
    for (int i = 0; i < 15; i++) {
        if ((self->graph_vertices & (1 << i)) != 0) {
            count++;
        }
    }
    return PyLong_FromSsize_t(count);
}

static PyObject*
vertices_get(AdjacencyMatrix* self, PyObject* Py_UNUSED(ignored))
{
    PyObject* list = PySet_New(0);    
    for (int i = 0; i < 16; i++) {
        if ((self->graph_vertices & (1 << i)) != 0) {
            PySet_Add(list, PyLong_FromSsize_t(i));
        }
    }        
    return list;
}


static PyObject*
vertex_degree(AdjacencyMatrix* self, PyObject* index)
{    
    Py_ssize_t degree = 0;
    int ind = PyLong_AsLong(index);

    for (int j = 0; j < ind; j++) {
        if ((self->graph_adjacencyMatrix[ind] & (1 << j)) != 0) {
            degree++;
        }
    }
    for (int i = ind+1; i < 16; i++) {
        if ((self->graph_adjacencyMatrix[i] & (1 << ind)) != 0) {
            degree++;
        }
    }
    return PyLong_FromSsize_t(degree);
}

static PyObject*
vertex_neighbors(AdjacencyMatrix* self, PyObject* index)
{
    PyObject* list = PySet_New(0);
    int ind = PyLong_AsLong(index);
    for (int j = 0; j < ind; j++) {
        if ((self->graph_adjacencyMatrix[ind] & (1 << j)) != 0) {
            PySet_Add(list, PyLong_FromSsize_t(j));
        }
    }
    for (int i = ind + 1; i < 16; i++) {
        if ((self->graph_adjacencyMatrix[i] & (1 << ind)) != 0) {
            PySet_Add(list, PyLong_FromSsize_t(i));
        }
    }
    return list;
}

static PyObject*
add_vertex(AdjacencyMatrix* self, PyObject* index)
{
    int ind = PyLong_AsLong(index);    
    self->graph_vertices |= (1 << ind);    
    Py_RETURN_NONE;
}

static PyObject*
delete_vertex(AdjacencyMatrix* self, PyObject* index)
{
    int ind = PyLong_AsLong(index);
    self->graph_vertices &= ~(1 << ind);
    self->graph_adjacencyMatrix[ind] = 0;    
    for (int i = ind + 1; i < 16; i++) {
        if ((self->graph_adjacencyMatrix[i] & (1 << ind)) != 0) {
            self->graph_adjacencyMatrix[i] &= ~(1 << ind);
        }
    }
    Py_RETURN_NONE;
}

static PyObject*
number_of_edges(AdjacencyMatrix* self, PyObject* Py_UNUSED(ignored))
{
    int count = 0;
    for (int i = 1; i < 15; i++) {        
        for (int j = 0; j < i; j++) {                        
            if (((self->graph_adjacencyMatrix[i]) & ( 1 << j)) != 0) {                
                count++;
            }
        }
    }
    return PyLong_FromSsize_t(count);
}

static PyObject*
edges(AdjacencyMatrix* self, PyObject* Py_UNUSED(ignored))
{
    PyObject* list = PySet_New(NULL);
    for (int i = 1; i < 16; i++) {
        for (int j = 0; j < i; j++) {
            if (((self->graph_adjacencyMatrix[i]) & (1 << j)) != 0) {
                PyObject* tuple = PyTuple_New(2);                
                PyTuple_SetItem(tuple, 0, PyLong_FromLong(j));                
                PyTuple_SetItem(tuple, 1, PyLong_FromLong(i));                
                PySet_Add(list, tuple);                
            }
        }
    }
    return list;
}

static PyObject*
is_edge(AdjacencyMatrix* self, PyObject* args, PyObject* kwargs)
{
    static char* kwlist[] = { "i", "j", NULL };    
    int i,j;    
 
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "ii", kwlist, &i, &j)) {        
        Py_RETURN_NONE;
    }            

    if (i > j) {
        int temp = i;
        i = j;
        j = temp;
    }
    return  (((self->graph_adjacencyMatrix[j]) & (1 << i)) != 0)?
            PyBool_FromLong(1):
            PyBool_FromLong(0);
}


static PyObject*
add_edge(AdjacencyMatrix* self, PyObject* args, PyObject* kwargs)
{
    
    static char* kwlist[] = { "i", "j", NULL };
    int i, j;
        
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "ii", kwlist, &i, &j)) {        
        Py_RETURN_NONE;
    }    
    AddEdge(self, i, j);
    Py_RETURN_NONE;
}

static PyObject*
delete_edge(AdjacencyMatrix* self, PyObject* args, PyObject* kwargs)
{
    static char* kwlist[] = { "i", "j", NULL };
    int i, j;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "ii", kwlist, &i, &j)) {
        Py_RETURN_NONE;
    }
        

    self->graph_adjacencyMatrix[i] &= ~( 1 << j);    
    self->graph_adjacencyMatrix[j] &= ~( 1 << i);
    
    Py_RETURN_NONE;
}



static PyMethodDef Custom_methods[] = {
    {"number_of_vertices", (PyCFunction)number_of_vertices, METH_NOARGS,""},
    {"vertices", (PyCFunction)vertices_get, METH_NOARGS,""},
    {"vertex_degree", (PyCFunction)vertex_degree, METH_O,""},
    {"vertex_neighbors", (PyCFunction)vertex_neighbors, METH_O,""},
    {"add_vertex", (PyCFunction)add_vertex, METH_O,""},
    {"delete_vertex", (PyCFunction)delete_vertex, METH_O,""},
    {"number_of_edges", (PyCFunction)number_of_edges, METH_NOARGS,""},
    {"edges", (PyCFunction)edges, METH_NOARGS,""},
    {"is_edge", (PyCFunction)is_edge, METH_VARARGS | METH_KEYWORDS,""},
    {"add_edge", (PyCFunction)add_edge, METH_VARARGS | METH_KEYWORDS,""},
    {"delete_edge", (PyCFunction)delete_edge, METH_VARARGS | METH_KEYWORDS,""},
    {"print_matrix", (PyCFunction)print_matrix, METH_NOARGS,""},
    {"create_wheel", (PyCMethod)WheelInit, METH_METHOD | METH_FASTCALL | METH_KEYWORDS | METH_CLASS  ,""},
    {NULL}  /* Sentinel */
};
#pragma endregion METHODS

static PyTypeObject AdjacencyMatrixType = {
    .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "AdjacencyMatrixType",
    .tp_doc = PyDoc_STR("Custom objects"),
    .tp_basicsize = sizeof(AdjacencyMatrix),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new = Custom_new,
    .tp_init = (initproc)Custom_init,
    .tp_dealloc = (destructor)Custom_dealloc,
    .tp_members = Custom_members,
    .tp_methods = Custom_methods,    
};

static PyModuleDef custommodule = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "AdjacencyMatrixModule",
    .m_doc = "AdjacencyMatrix module made by Jakub Wierzba.",
    .m_size = -1,
};

PyMODINIT_FUNC
PyInit_simple_graphs(void)
{
    PyObject* m;
    if (PyType_Ready(&AdjacencyMatrixType) < 0)
        return NULL;

    m = PyModule_Create(&custommodule);
    if (m == NULL)
        return NULL;

    if (PyModule_AddObjectRef(m, "AdjacencyMatrix", (PyObject*)&AdjacencyMatrixType) < 0) {
        Py_DECREF(m);
        return NULL;
    }
    
    return m;
}