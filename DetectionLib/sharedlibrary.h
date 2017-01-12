#ifndef SHAREDLIBRARY_H
#define SHAREDLIBRARY_H

#include <QtCore/QtGlobal>

#if defined(SHAREDLIB_LIBRARY_DEF)
#define SHAREDLIB_LIBRARY Q_DECL_EXPORT
#else
#define SHAREDLIB_LIBRARY Q_DECL_IMPORT
#endif

#endif // SHAREDLIBRARY_H
