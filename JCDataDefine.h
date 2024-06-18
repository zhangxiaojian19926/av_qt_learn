#ifndef JCDATADEFINE_H
#define JCDATADEFINE_H

#include <QList>
#include <QMutex>


// chuangjian duilie wanchengshujuhuancun
template <class T>
class JCTSQueue : public QList<T>
{
private:
    QMutex m_mutex;

public:
    JCTSQueue()
    {

    }

    ~JCTSQueue()
    {

    }

    void enquue(const T &t)
    {
        m_mutex.lock();
        QList<T>::append(t);
        m_mutex.unlock();
    }

    T dequeue()
    {
        m_mutex.lock();
        T t = NULL;
        if(!QList<T>::isEmpty())
        {
            t = QList<T>::takeFirst();
        }

        m_mutex.unlock();
        return t;
    }

    bool isEmpty()
    {
        m_mutex.lock();
        bool b = QList<T>::isEmpty();
        m_mutex.unlock();
        return b;
    }
};

#endif // JCDATADEFINE_H
