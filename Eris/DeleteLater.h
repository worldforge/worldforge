


namespace Eris
{

class BaseDeleteLater
{
public:
	virtual void execute() = 0;
};


template <class T>
class DerivedDeleteLater : public BaseDeleteLater
{
public:
	DerivedDeleteLater(T* ins) : m_instance(ins) { }
	
	virtual void execute() { delete m_instance; }
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
