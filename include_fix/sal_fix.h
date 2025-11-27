// SAL (Source Annotation Language) compatibility for MinGW
// These are MSVC-specific annotations that MinGW doesn't support
// Define them as empty macros

#ifndef _SAL_FIX_H_
#define _SAL_FIX_H_

// IMPORTANT: Include C++ STL headers BEFORE defining SAL macros
// because STL internally uses __in, __out etc. as parameter names
// and our empty macro definitions would break them
#ifdef __cplusplus
#include <algorithm>
#include <utility>
#include <type_traits>
#endif

// Basic SAL annotations
#ifndef __in
#define __in
#endif

#ifndef __out
#define __out
#endif

#ifndef __inout
#define __inout
#endif

#ifndef __in_opt
#define __in_opt
#endif

#ifndef __out_opt
#define __out_opt
#endif

#ifndef __inout_opt
#define __inout_opt
#endif

// Deref annotations
#ifndef __deref_in
#define __deref_in
#endif

#ifndef __deref_out
#define __deref_out
#endif

#ifndef __deref_inout
#define __deref_inout
#endif

#ifndef __deref_in_opt
#define __deref_in_opt
#endif

#ifndef __deref_out_opt
#define __deref_out_opt
#endif

#ifndef __deref_inout_opt
#define __deref_inout_opt
#endif

// Count annotations
#ifndef __in_ecount
#define __in_ecount(x)
#endif

#ifndef __out_ecount
#define __out_ecount(x)
#endif

#ifndef __inout_ecount
#define __inout_ecount(x)
#endif

#ifndef __in_ecount_opt
#define __in_ecount_opt(x)
#endif

#ifndef __out_ecount_opt
#define __out_ecount_opt(x)
#endif

#ifndef __inout_ecount_opt
#define __inout_ecount_opt(x)
#endif

// Byte count annotations
#ifndef __in_bcount
#define __in_bcount(x)
#endif

#ifndef __out_bcount
#define __out_bcount(x)
#endif

#ifndef __inout_bcount
#define __inout_bcount(x)
#endif

#ifndef __in_bcount_opt
#define __in_bcount_opt(x)
#endif

#ifndef __out_bcount_opt
#define __out_bcount_opt(x)
#endif

#ifndef __inout_bcount_opt
#define __inout_bcount_opt(x)
#endif

// Other common annotations
#ifndef __reserved
#define __reserved
#endif

#ifndef __notnull
#define __notnull
#endif

#ifndef __maybenull
#define __maybenull
#endif

#ifndef __field_ecount
#define __field_ecount(x)
#endif

#ifndef __field_bcount
#define __field_bcount(x)
#endif

#ifndef __field_ecount_opt
#define __field_ecount_opt(x)
#endif

#ifndef __control_entrypoint
#define __control_entrypoint(x)
#endif

#ifndef __callback
#define __callback
#endif

// Range annotations
#ifndef __deref_out_range
#define __deref_out_range(x,y)
#endif

#ifndef __range
#define __range(x,y)
#endif

#ifndef __out_range
#define __out_range(x,y)
#endif

#ifndef __in_range
#define __in_range(x,y)
#endif

// Success/failure annotations
#ifndef __success
#define __success(x)
#endif

#ifndef __checkReturn
#define __checkReturn
#endif

#endif // _SAL_FIX_H_
