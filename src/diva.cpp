#include "diva.h"

namespace diva {
FUNCTION_PTR (void *, operatorNew, 0x1409777D0, u64);
FUNCTION_PTR (void *, operatorDelete, 0x1409B1E90, void *);
FUNCTION_PTR (void, FreeString, 0x14014BCD0, string *);
FUNCTION_PTR (bool, ResolveFilePath, 0x1402A5330, string *from, string *out);
FUNCTION_PTR (bool, FileRequestLoad, 0x1402A4710, void *fileHandler, char *file, bool);
FUNCTION_PTR (bool, FileCheckNotReady, 0x151C03830, void *fileHandler);
FUNCTION_PTR (const void *, FileGetData, 0x151C0EF70, void *fileHandler);
FUNCTION_PTR (size_t, FileGetSize, 0x151C7ADA0, void *fileHandler);

vector<string> *romDirs = (vector<string> *)0x1414AB8A0;
} // namespace diva
