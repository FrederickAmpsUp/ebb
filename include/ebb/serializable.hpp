#pragma once

#include <stdio.h>

namespace Ebb {
    /**
     * Abstract class for serializable objects (objects that may be saved to disk.)
    */
class Serializable {
public:
    /**
     * Save a serializable object to disk.
     * This should save all data in the serializable object, including any child objects
     * 
     * @param file The file to write to.
     * @retval void
    */
    virtual void save(FILE *file) = 0;

    /**
     * Read a serializable object from disk.
     * The object must be initialized before calling this method.
     * @param file The file to read from.
     * @retval void
    */
    virtual void load(FILE *file) = 0;
};
};