define(`FORLOOP', `ifelse(eval($2 > $3), `1', , `pushdef(`$1', `$2')_FORLOOP(`$1', `$2', `$3', `$4')popdef(`$1')')')dnl
define(`_FORLOOP', `$4`'ifelse($1, `$3', , `define(`$1', incr($1))_FORLOOP(`$1', `$2', `$3', `$4')')')dnl
define(`TEMPLATE', `template<`'FORLOOP(`i', 1, eval($1 - 1), `typename T`'i, ')`'typename T`'$1`'>')dnl
define(`CONSTRUCTOR', `
    Morph`'$1`'()
      : `'FORLOOP(`i', 1, eval($1 - 1), `v`'i`'(NULL), ') v$1(NULL)
    {
    }')dnl
define(`CONSTRUCTOR_T', `
    Morph`'$1`'(const T`'$2& v)
      : `'FORLOOP(`i', 1, eval($2 - 1), `v`'i`'(NULL), ')`'v$2(new T$2(v))`'FORLOOP(`i', eval($2 + 1), eval($1 - 1), `, v`'i`'(NULL)')`'ifelse($1, $2, , `, v$1(NULL)')
    {
    }')dnl
define(`DESTRUCTOR', `
    virtual ~Morph$1()
    {`'FORLOOP(`i', 1, $1, `
        delete v`'i`';')
    }')dnl
define(`OP_ASSIGN', `
    Morph$1& operator=(const Morph$1& m)
    {
        Clear();FORLOOP(`i', 1, $1, `
        if (m.v`'i`' != NULL) *this = *m.v`'i`';')
        return *this;
    }')dnl
define(`OP_ASSIGN_T', `
    Morph$1& operator=(const T$2& v)
    {
        Clear();
        v$2 = new T$2;
        *v$2 = v;
        return *this;
    }')dnl
define(`OP_EQ', `
    bool operator!=(const Morph$1& m) const
    {`'FORLOOP(`i', 1, $1, `
        if (v`'i`' == NULL) {
            if (m.v`'i`' != NULL) return true;
        } else if (*v`'i`' != *m.v`'i`') return true;')
        
        return false;
    }

    bool operator==(const Morph$1 m) const
    {
        return !(*this != m);
    }')dnl
define(`OP_EQ_T', `
    bool operator!=(const T$1& v) const
    {
        if (v$1 == NULL || *v$1 != v)  return true;
        return false;
    }

    bool operator==(const T$1& v) const { return !(*this != v); }
')dnl
define(`CLEAR', `
    void Clear()
    {`'FORLOOP(`i', 1, $1, `
        if (v`'i`' != NULL) { delete v`'i`'; v`'i`' = NULL; }')
    }')dnl
define(`IS_T', `
    bool Is(Token<T$1>) { return (v$1 != NULL); }')dnl
define(`AS_T', `
    T$1 As(Token<T$1>) throw (WrongTypeException)
    {
        if (v$1 != NULL) return *v$1;
        throw WrongTypeException();
    }')dnl
define(`MEMBERS', `FORLOOP(`i', 1, $1, `
    T`'i`'* v`'i`';')')dnl
define(`MORPH', `

TEMPLATE($1)
class Morph$1
{
public:
CONSTRUCTOR($1)
FORLOOP(`n', 1, $1, `CONSTRUCTOR_T($1, n)')
DESTRUCTOR($1)
OP_ASSIGN($1)
FORLOOP(`n', 1, $1, `OP_ASSIGN_T($1, n)')
OP_EQ($1)
FORLOOP(`n', 1, $1, `OP_EQ_T(n)')
CLEAR($1)
FORLOOP(`n', 1, $1, `IS_T(n)')
FORLOOP(`n', 1, $1, `AS_T(n)')

protected:
MEMBERS($1)

};
')dnl
