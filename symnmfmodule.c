#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "symnmf.h"


static PyObject* symnmf(PyObject *self, PyObject *args);
static PyObject* sym(PyObject *self, PyObject *args);
static PyObject* ddg(PyObject *self, PyObject *args);
static PyObject* norm(PyObject *self, PyObject *args);


static PyObject* symnmf(PyObject *self, PyObject *args){

}


static PyObject* sym(PyObject *self, PyObject *args){

}


static PyObject* ddg(PyObject *self, PyObject *args){

}


static PyObject* norm(PyObject *self, PyObject *args){

}


static PyMethodDef matrixMethods[] = {
    {"symnmf",
    (PyCFunction) symnmf,
    METH_VARARGS,
    PyDoc_STR("Executing the SymNMF algoritm")},

    {"sym",
    (PyCFunction) sym,
    METH_VARARGS,
    PyDoc_STR("Computing the similarity matrix")},

    {"ddg",
    (PyCFunction) ddg,
    METH_VARARGS,
    PyDoc_STR("Computing the diagonal degerr matrix")},

    {"norm",
    (PyCFunction) norm,
    METH_VARARGS,
    PyDoc_STR("Computing the normalized similarity matrix")},
    {NULL, NULL, 0, NULL}

};


static struct PyModuleDef symnmfmodule = {
    PyModuleDef_HEAD_INIT,
    "SymNMF",
    NULL,
    -1,
    matrixMethods
};


PyMODINIT_FUNC PyInit_SymNMF(void){
    PyObject *m;
    m = PyModule_Create(&symnmfmodule);
    if (!m){
        return NULL;
    }
    return m;
}


