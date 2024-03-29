//-----------------------------------------------------------------------------
//
// Copyright (c) 1998 - 2007, The Regents of the University of California
// Produced at the Lawrence Livermore National Laboratory
// All rights reserved.
//
// This file is part of PyCXX. For details,see http://cxx.sourceforge.net/. The
// full copyright notice is contained in the file COPYRIGHT located at the root
// of the PyCXX distribution.
//
// Redistribution  and  use  in  source  and  binary  forms,  with  or  without
// modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of  source code must  retain the above  copyright notice,
//    this list of conditions and the disclaimer below.
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this  list of  conditions  and  the  disclaimer (as noted below)  in  the
//    documentation and/or materials provided with the distribution.
//  - Neither the name of the UC/LLNL nor  the names of its contributors may be
//    used to  endorse or  promote products derived from  this software without
//    specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR  IMPLIED WARRANTIES, INCLUDING,  BUT NOT  LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND  FITNESS FOR A PARTICULAR  PURPOSE
// ARE  DISCLAIMED.  IN  NO  EVENT  SHALL  THE  REGENTS  OF  THE  UNIVERSITY OF
// CALIFORNIA, THE U.S.  DEPARTMENT  OF  ENERGY OR CONTRIBUTORS BE  LIABLE  FOR
// ANY  DIRECT,  INDIRECT,  INCIDENTAL,  SPECIAL,  EXEMPLARY,  OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT  LIMITED TO, PROCUREMENT OF  SUBSTITUTE GOODS OR
// SERVICES; LOSS OF  USE, DATA, OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER
// CAUSED  AND  ON  ANY  THEORY  OF  LIABILITY,  WHETHER  IN  CONTRACT,  STRICT
// LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE)  ARISING IN ANY  WAY
// OUT OF THE  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.
//
//-----------------------------------------------------------------------------

#ifndef CXX_ExtensionClass_h
#define CXX_ExtensionClass_h

#include <cstddef>

#define PYCXX_NOARGS_METHOD_NAME( NAME ) _callNoArgsMethod__##NAME
#define PYCXX_VARARGS_METHOD_NAME( NAME ) _callVarArgsMethod__##NAME
#define PYCXX_KEYWORDS_METHOD_NAME( NAME ) _callKeywordsMethod__##NAME

#define PYCXX_NOARGS_METHOD_DECL( CLS, NAME ) \
    static PyObject *PYCXX_NOARGS_METHOD_NAME( NAME )( PyObject *_self, PyObject * ) \
    { \
        try \
        { \
            auto *self_python = reinterpret_cast< Py::PythonClassInstance * >( _self ); \
            auto *self = reinterpret_cast< CLS * >( self_python->m_pycxx_object ); \
            Py::Object r( (self->NAME)() ); \
            return Py::new_reference_to( r.ptr() ); \
        } \
        catch( Py::BaseException & ) \
        { \
            return nullptr; \
        } \
    }
#define PYCXX_VARARGS_METHOD_DECL( CLS, NAME ) \
    static PyObject *PYCXX_VARARGS_METHOD_NAME( NAME )( PyObject *_self, PyObject *_a ) \
    { \
        try \
        { \
            auto *self_python = reinterpret_cast< Py::PythonClassInstance * >( _self ); \
            auto *self = reinterpret_cast< CLS * >( self_python->m_pycxx_object ); \
            Py::Tuple a( _a ); \
            Py::Object r( (self->NAME)( a ) ); \
            return Py::new_reference_to( r.ptr() ); \
        } \
        catch( Py::BaseException & ) \
        { \
            return nullptr; \
        } \
    }
#define PYCXX_KEYWORDS_METHOD_DECL( CLS, NAME ) \
    static PyObject *PYCXX_KEYWORDS_METHOD_NAME( NAME )( PyObject *_self, PyObject *_a, PyObject *_k ) \
    { \
        try \
        { \
            Py::PythonClassInstance *self_python = reinterpret_cast< Py::PythonClassInstance * >( _self ); \
            CLS *self = reinterpret_cast< CLS * >( self_python->m_pycxx_object ); \
            Py::Tuple a( _a ); \
            Py::Dict k; \
            if( _k != nullptr ) \
                k = _k; \
            Py::Object r( (self->NAME)( a, k ) ); \
            return Py::new_reference_to( r.ptr() ); \
        } \
        catch( Py::BaseException & ) \
        { \
            return nullptr; \
        } \
    }

// need to support METH_STATIC and METH_CLASS

/**
 * Python doesn't really support providing signatures in the meta data for built-in functions (i.e. any C/C++ extensions).
 * There's however a slightly undocumented feature that if a docstring is provided in a certain format, the first part will be used as the signature.
 * The docstring must contain on its first line a signature, without any annotations (which is a real shame), followed by "\n--\n\n" and then the docstring.
 * Consider a method "foo(self, aNumber)".
 * If the docstring looks like this it will work:
 * """
 * foo(self, aNumber)
 * --
 *
 * A description of what foo does.
 * """
 *
 * Note the lack of support for annotations. This would otherwise have allowed us to specify the expected types, as well as any return types. Perhaps in a future Python version...
 */
#define PYCXX_SIG_DOC( signature, doc ) signature "\n--\n\n" doc


#define PYCXX_ADD_NOARGS_METHOD( PYNAME, NAME, docs ) \
    add_method( #PYNAME, (PyCFunction)PYCXX_NOARGS_METHOD_NAME( NAME ), METH_NOARGS, docs )
#define PYCXX_ADD_VARARGS_METHOD( PYNAME, NAME, docs ) \
    add_method( #PYNAME, (PyCFunction)PYCXX_VARARGS_METHOD_NAME( NAME ), METH_VARARGS, docs )
#define PYCXX_ADD_KEYWORDS_METHOD( PYNAME, NAME, docs ) \
    add_method( #PYNAME, (PyCFunction)PYCXX_KEYWORDS_METHOD_NAME( NAME ), METH_VARARGS | METH_KEYWORDS, docs )

namespace Py
{
    extern PythonExtensionBase *getPythonExtensionBase( PyObject *self );
    struct PythonClassInstance
    {
        PyObject_HEAD
        PythonExtensionBase *m_pycxx_object;

        static void initType(PythonType* p) {}
        static void dealloc(PyObject* _self, PythonClassInstance* self) {}
        static void init(PyObject* _self, PythonClassInstance* self) {}
        static void _new(PythonClassInstance* o) {}


    };


    //Allows for weak references. This struct must look just like PythonClassInstance data wise,
    //to allow for it to be reinterpret_casted to PythonClassInstance.
    struct PythonClassInstanceWeak
    {
        PyObject_HEAD
        PythonExtensionBase *m_pycxx_object;
        PyObject * m_weakreflist;

        static void initType(PythonType* p){
            p->type_object()->tp_weaklistoffset = offsetof(PythonClassInstanceWeak, m_weakreflist);
        }
        static void dealloc(PyObject* _self, PythonClassInstanceWeak* self) {
            if (self->m_weakreflist != nullptr) {
                PyObject_ClearWeakRefs(_self);
            }
        }
        static void init(PyObject* _self, PythonClassInstanceWeak* self) {
            self->m_weakreflist = nullptr;
        }
        static void _new(PythonClassInstanceWeak* o) {
            o->m_weakreflist = nullptr;
        }

    };

    //Check that both are Standard Layout, so that both can be reinterpret_casted to PythonClassInstance (without inheriting).
    static_assert(std::is_standard_layout<PythonClassInstance>(), "PythonClassInstance must be standard layout.");
    static_assert(std::is_standard_layout<PythonClassInstanceWeak>(), "PythonClassInstanceWeak must be standard layout.");

    class ExtensionClassMethodsTable
    {
    public:
        ExtensionClassMethodsTable()
        : m_methods_table( new PyMethodDef[ METHOD_TABLE_SIZE_INCREMENT ] )
        , m_methods_used( 0 )
        , m_methods_size( METHOD_TABLE_SIZE_INCREMENT )
        {
            // add the sentinel marking the table end
            PyMethodDef *p = &m_methods_table[ 0 ];

            p->ml_name = nullptr;
            p->ml_meth = nullptr;
            p->ml_flags = 0;
            p->ml_doc = nullptr;
        }

        ~ExtensionClassMethodsTable()
        {
            delete[] m_methods_table;
        }

        // check that all methods added are unique
        void check_unique_method_name( const char *_name )
        {
            std::string name( _name );
            for( int i=0; i<m_methods_used; i++ )
            {
                if( name == m_methods_table[i].ml_name )
                {
                    throw AttributeError( name );
                }
            }
        }
        PyMethodDef *add_method( const char *name, PyCFunction function, int flags, const char *doc )
        {
            check_unique_method_name( name );

            // see if there is enough space for one more method
            if( m_methods_used == (m_methods_size-1) )
            {
                PyMethodDef *old_mt = m_methods_table;
                m_methods_size += METHOD_TABLE_SIZE_INCREMENT;
                PyMethodDef *new_mt = new PyMethodDef[ m_methods_size ];
                for( int i=0; i<m_methods_used; i++ )
                {
                    new_mt[ i ] = old_mt[ i ];
                }
                delete[] old_mt;
                m_methods_table = new_mt;
            }

            // add method into the table
            PyMethodDef *p = &m_methods_table[ m_methods_used ];
            p->ml_name = name;
            p->ml_meth = function;
            p->ml_flags = flags;
            p->ml_doc = doc;

            m_methods_used++;
            p++;

            // add the sentinel marking the table end
            p->ml_name = nullptr;
            p->ml_meth = nullptr;
            p->ml_flags = 0;
            p->ml_doc = nullptr;

            return m_methods_table;
        }

    private:
        enum {METHOD_TABLE_SIZE_INCREMENT = 1};
        PyMethodDef *m_methods_table;
        int m_methods_used;
        int m_methods_size;
    };

    template<TEMPLATE_TYPENAME T, TEMPLATE_TYPENAME TClassInstance = PythonClassInstance> class PythonClass
    : public PythonExtensionBase
    {
        public:
            typedef TClassInstance ClassInstance;

    protected:

        explicit PythonClass( TClassInstance *self, const Tuple &/*args*/, const Dict &/*kwds*/ )
        : PythonExtensionBase()
        , m_class_instance( self )
        {
        }

        explicit PythonClass( TClassInstance *self)
            : PythonExtensionBase()
            , m_class_instance( self )
        {
        }

        virtual ~PythonClass() = default;

        static ExtensionClassMethodsTable &methodTable()
        {
            static ExtensionClassMethodsTable *method_table;
            if( method_table == nullptr )
                method_table = new ExtensionClassMethodsTable;
            return *method_table;
        }

        static void add_method( const char *name, PyCFunction function, int flags, const char *doc=nullptr )
        {
            behaviors().set_methods( methodTable().add_method( name, function, flags, doc ) );
        }

        static PythonType &behaviors()
        {
            static PythonType *p;
            if( p == nullptr )
            {
#if defined( _CPPRTTI ) || defined( __GNUG__ )
                const char *default_name = (typeid( T )).name();
#else
                const char *default_name = "unknown";
#endif
                p = new PythonType( sizeof( TClassInstance ), 0, default_name );
                TClassInstance::initType(p);
                p->set_tp_new( extension_object_new );
                p->set_tp_init( extension_object_init );
                p->set_tp_dealloc( extension_object_deallocator );

                // we are a class
                p->supportClass();

                // always support get and set attr
                p->supportGetattro();
                p->supportSetattro();
            }

            return *p;
        }

        static PyObject *extension_object_new( PyTypeObject *subtype, PyObject * /*args*/, PyObject * /*kwds*/ )
        {
#ifdef PYCXX_DEBUG
            std::cout << "extension_object_new()" << std::endl;
#endif
            auto *o = reinterpret_cast<TClassInstance *>( subtype->tp_alloc( subtype, 0 ) );
            if( o == nullptr )
                return nullptr;

            o->m_pycxx_object = nullptr;
            TClassInstance::_new(o);

            PyObject *self = reinterpret_cast<PyObject *>( o );
#ifdef PYCXX_DEBUG
            std::cout << "extension_object_new() => self=0x" << std::hex << reinterpret_cast< unsigned long >( self ) << std::dec << std::endl;
#endif
            return self;
        }

        static int extension_object_init( PyObject *_self, PyObject *args_, PyObject *kwds_ )
        {
            try
            {
                Py::Tuple args( args_ );
                Py::Dict kwds;
                if( kwds_ != nullptr )
                    kwds = kwds_;

                auto*self = reinterpret_cast<TClassInstance *>( _self );
#ifdef PYCXX_DEBUG
                std::cout << "extension_object_init( self=0x" << std::hex << reinterpret_cast< unsigned long >( self ) << std::dec << " )" << std::endl;
                std::cout << "    self->m_pycxx_object=0x" << std::hex << reinterpret_cast< unsigned long >( self->m_pycxx_object ) << std::dec << std::endl;
#endif

                if( self->m_pycxx_object == nullptr )
                {
                    self->m_pycxx_object = new T( self, args, kwds );
                    TClassInstance::init(_self, self);
#ifdef PYCXX_DEBUG
                    std::cout << "    self->m_pycxx_object=0x" << std::hex << reinterpret_cast< unsigned long >( self->m_pycxx_object ) << std::dec << std::endl;
#endif
                }
                else
                {
#ifdef PYCXX_DEBUG
                    std::cout << "    reinit - self->m_pycxx_object=0x" << std::hex << reinterpret_cast< unsigned long >( self->m_pycxx_object ) << std::dec << std::endl;
#endif
                    self->m_pycxx_object->reinit( args, kwds );
                }
            }
            catch( BaseException & )
            {
                return -1;
            }
            return 0;
        }

        static void extension_object_deallocator( PyObject *_self )
        {
            TClassInstance *self = reinterpret_cast< TClassInstance * >( _self );
#ifdef PYCXX_DEBUG
            std::cout << "extension_object_deallocator( self=0x" << std::hex << reinterpret_cast< unsigned long >( self ) << std::dec << " )" << std::endl;
            std::cout << "    self->m_pycxx_object=0x" << std::hex << reinterpret_cast< unsigned long >( self->m_pycxx_object ) << std::dec << std::endl;
#endif
            TClassInstance::dealloc(_self, self);
            delete self->m_pycxx_object;
            _self->ob_type->tp_free( _self );
        }

    public:
        static PyTypeObject *type_object()
        {
            return behaviors().type_object();
        }

        static Object type()
        {
            return Object( reinterpret_cast<PyObject *>( behaviors().type_object() ) );
        }

        static bool check( PyObject *p )
        {
            // is p a me or a derived me
            switch( PyObject_IsInstance( p, reinterpret_cast<PyObject *>( type_object() ) ) )
            {
                default:
                case -1:
                    throw Exception();
                case 0:
                    return false;
                case 1:
                    return true;
            }
        }

        static bool check( const Object &ob )
        {
            return check( ob.ptr() );
        }

        virtual PyObject *selfPtr()
        {
            return reinterpret_cast<PyObject *>( m_class_instance );
        }

        virtual Object self()
        {
            return Object( reinterpret_cast<PyObject *>( m_class_instance ) );
        }

    protected:
    private:
            TClassInstance *m_class_instance;

    private:
        //
        // prevent the compiler generating these unwanted functions
        //
        explicit PythonClass( const PythonClass<T> &other );
        void operator=( const PythonClass<T> &rhs );
    };

    //
    // ExtensionObject<T> is an Object that will accept only T's.
    //
    template<TEMPLATE_TYPENAME T>
    class PythonClassObject: public Object
    {
    public:

        explicit PythonClassObject( PyObject *pyob, bool owned = false  )
        : Object( pyob, owned )
        {
            validate();
        }

        PythonClassObject( const PythonClassObject<T> &other )
        : Object( *other )
        {
            validate();
        }

        PythonClassObject( const Object &other )
        : Object( *other )
        {
            validate();
        }

        PythonClassObject &operator=( const Object &rhs )
        {
            *this = *rhs;
            return *this;
        }

        PythonClassObject &operator=( PyObject *rhsp )
        {
            if( ptr() != rhsp )
                set( rhsp );
            return *this;
        }

        virtual bool accepts( PyObject *pyob ) const
        {
            return( pyob && T::check( pyob ) );
        }

        //
        //    Obtain a pointer to the PythonExtension object
        //
        T *getCxxObject( void )
        {
            return dynamic_cast< T * >( getPythonExtensionBase( ptr() ) );
        }

        static T *getCxxObject(const Py::Object& obj )
        {
            auto ptr = obj.ptr();
            if (!ptr || !T::check(ptr)) {
                std::string s( "PyCXX: Error creating object of type " );
                s += (typeid( T )).name();

                if( ptr )
                {
                    String from_repr = obj.repr();
                    s += " from ";
                    s += from_repr.as_std_string();
                }
                else
                {
                    s += " from (nil)";
                }
                throw TypeError( s );
            }
            return dynamic_cast< T * >( getPythonExtensionBase( ptr ) );
        }

    };
} // Namespace Py

// End of CXX_ExtensionClass_h
#endif
