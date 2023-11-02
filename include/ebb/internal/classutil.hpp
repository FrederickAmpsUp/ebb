/**
 * @brief Define a getter for a variable. Should be only used in a private context.
 * @param name The name of the variable to define a getter, should be underscore-prefixed
 */
#define getter(name) \
public:              \
    decltype(name) get##name () { return name; } \
private:

/**
 * @brief Define a setter for a variable. Should be only used in a private context.
 * @param name The name of the variable to define a setter, should be underscore-prefixed
*/
#define setter(name) \
public:              \
    void set##name (decltype(name) value) { name = value; } \
private: