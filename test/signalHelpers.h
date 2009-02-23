#ifndef TEST_SIGNAL_HELPERS_H
#define TEST_SIGNAL_HELPERS_H

class SignalCounter0
{
public:
    SignalCounter0() :
        m_count(0)
    {;}
    
    void fired()
    {
        ++m_count;
    }
    
    int fireCount() const
    { return m_count; }
    
    void reset()
    {
        m_count = 0;
    }
protected:
    int m_count;
};

template <class P0>
class SignalCounter1 : public SignalCounter0
{
public:

    void fired(P0)
    {
        ++m_count;
    }
};

template <class P0, class P1>
class SignalCounter2 : public SignalCounter0
{
public:

    void fired(P0, P1)
    {
        ++m_count;
    }

};
template <class P0>
class SignalRecorder1
{
public:
    SignalRecorder1() :
        m_count(0)
    {;}
    
    void fired(P0 m)
    {
        ++m_count;
        m_lastArg0 = m;
    }
    
    int fireCount() const
    { return m_count; }
    
    P0 lastArg0() const
    {
        return m_lastArg0;
    }
    
    void reset()
    {
        m_count = 0;
    }
private:
    int m_count;
    P0 m_lastArg0;
};

template <class P0>
class SignalRecorderRef1
{
public:
    SignalRecorderRef1() :
        m_count(0)
    {;}
    
    void fired(const P0& m)
    {
        ++m_count;
        m_lastArg0 = m;
    }
    
    int fireCount() const
    { return m_count; }
    
    P0 lastArg0() const
    {
        return m_lastArg0;
    }
    
    void reset()
    {
        m_count = 0;
    }
private:
    int m_count;
    P0 m_lastArg0;
};


#endif // of TEST_SIGNAL_HELPERS_H
