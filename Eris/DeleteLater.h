#ifndef ERIS_DELETE_LATER_H
#define ERIS_DELETE_LATER_H

namespace Eris
{

class BaseDeleteLater
{
public:
	virtual ~BaseDeleteLater() = 0;
};


template <class T>
class DerivedDeleteLater : public BaseDeleteLater
{
public:
	DerivedDeleteLater(T* ins) : m_instance(ins) { }
    virtual ~DerivedDeleteLater()
    {
        delete m_instance;
    }
    
private:
	T* m_instance;
};

void pushDeleteLater(BaseDeleteLater* bl);

void execDeleteLaters();

template <class T>
void deleteLater(T* ins)
{
	pushDeleteLater(new DerivedDeleteLater<T>(ins));
}

} // of namespace

#endif ERIS_DELETE_LATER_H
