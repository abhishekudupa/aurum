# Aurum
Aurum is a runtime library that includes the following:

1. A well-designed base class hierarchy that includes base classes that represent hashable objects, equatable objects, comparable objects and 'stringifiable' objects, with built-in support for Tuples and iterables. Overhead is kept to a minimum by avoiding dynamic dispatch where possible, insteading leveraging CRTP.
2. A reimplementation of most types in the STL, without all of the iterator weirdnesses: which implies that associative containers are implemented using open-addressing rather than chaining. Unfortunately, this also means that the container types are not drop-in replacements for STL containers.
3. A "stackable" streams library: filters can be stacked on streams. Built in support for basic compression filters. Additional user-defined filters are easy to implement as well.
4. A command-line option parser.
5. A memory manager, with pool allocators and small block allocators.
6. Lots of miscellaneous utilities: timers, heap-only objects, auto-only objects, interruptible methods, ref-counted objects, smart pointers, shell expansion utils, downcastables.
7. Most concepts generalize cleanly (and automagically) to Tuples, lists and iterables in general.
