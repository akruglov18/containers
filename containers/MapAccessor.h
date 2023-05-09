#include <atomic>
#include <mutex>

class MySharedMutex {
private:
    std::atomic<int> counter;
    bool flag = false;
    std::mutex mut;

public:
    MySharedMutex() = default;
    
    MySharedMutex(const MySharedMutex &other)
    {
    }

    void SharedLock()
    {
        while (true)
        {
            while (flag) {}
            counter++;
            if (flag)
            {
                counter--;
            }
            else
            {
                break;
            }
        }
    }

    void SharedRelease()
    {
        counter--;
    }

    void UniqueLock()
    {
        mut.lock();
        flag = true;
        while (counter != 0) {}
    }

    void UniqueRelease()
    {
        flag = false;
        mut.unlock();
    }

};

class UniqueAccessor
{
private:
    MySharedMutex *mut;
    MySharedMutex *mutCommon;
    bool wasMutInited = false;
    bool wasCommonMutInited = false;

public:

    void initCommon(MySharedMutex &lockMut)
    {
        if (wasCommonMutInited) return;
        
        mutCommon = &lockMut;
        lockMut.SharedLock();
        wasCommonMutInited = true;
    }

    void init(MySharedMutex &lockMut)
    {
        if (wasMutInited) return;
        
        mut = &lockMut;
        lockMut.UniqueLock();
        wasMutInited = true;
    }

    void releaseCommonMut()
    {
        if (wasCommonMutInited)
        {
            mutCommon->SharedRelease();
            wasCommonMutInited = false;
        }
    }

    void release()
    {
        if (wasCommonMutInited)
        {
            mutCommon->SharedRelease();
            wasCommonMutInited = false;
        }

        if (wasMutInited)
        {
            mut->UniqueRelease();
            wasMutInited = false;
        }   
    }
    
    ~UniqueAccessor()
    {
        release();
    }
};

class SharedAccessor
{
private:
    MySharedMutex *mut;
    MySharedMutex *mutCommon;
    bool wasMutInited = false;
    bool wasCommonMutInited = false;

public:
    SharedAccessor() {}

    void initCommon(MySharedMutex &lockMut)
    {
        if (wasCommonMutInited) return;
        
        mutCommon = &lockMut;
        lockMut.SharedLock();
        wasCommonMutInited = true;
    }

    void init(MySharedMutex &lockMut)
    {
        if (wasMutInited) return;
        
        mut = &lockMut;
        lockMut.SharedLock();
        wasMutInited = true;
    }

    void releaseCommonMut()
    {
        if (wasCommonMutInited)
        {
            mutCommon->SharedRelease();
            wasCommonMutInited = false;
        }
    }

    void release()
    {
        if (wasCommonMutInited)
        {
            mutCommon->SharedRelease();
            wasCommonMutInited = false;
        }
        
        if (wasMutInited)
        {
            mut->SharedRelease();
            wasMutInited = false;
        }
    }

    ~SharedAccessor()
    {
        release();
    }   
};
