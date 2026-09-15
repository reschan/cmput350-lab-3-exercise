#ifndef SHARED_PTR_HEADER
#define SHARED_PTR_HEADER

class ControlBlockBase {
public:
    ControlBlockBase() {}  // TODO: implement the default constructor.

    // dtor is virtual, so that we can call derived class's dtor from a ptr to this base class.
    virtual ~ControlBlockBase() {}  // TODO: implement the destructor.

    // pure virtual function; must be overriden by derived classes
    virtual void* managedAddress() = 0;

    // Delete copies, which also implicitly deletes moves.
    ControlBlockBase(const ControlBlockBase&) = delete;
    ControlBlockBase& operator=(const ControlBlockBase&) = delete;

    long increment()
    {
        // TODO: increment refcount by 1 and return result.
        refcount += 1;
        return refcount;
    }

    long decrement()
    {
        // TODO: decrement refcount by 1 and return result.
        refcount -= 1;
        return refcount;
    }

    long refCount() const
    {
        // TODO: just return the refcount.
        return refcount;
    }

private:
    // TODO: add field(s) which both control block types need to have
    long refcount;
};

template <typename T>
class ControlBlock : public ControlBlockBase {
public:
    ControlBlock(T* ptr) : mPtr{ptr} {}

    ~ControlBlock() override {
        if (mPtr != nullptr) {
            delete mPtr;
        }
    }

    void* managedAddress() override { return mPtr; }

private:
    T* mPtr;
    long refcount;
};


template <typename T>
class SharedPtr {
public:
    SharedPtr() : mPtr{nullptr}, mBlockPtr{nullptr} {}

    template <typename U>
    SharedPtr(U* ptr) : mPtr{static_cast<T*>(ptr)} {
        if (mBlockPtr == nullptr) {
            mBlockPtr = new ControlBlock(ptr);
        }
        mBlockPtr->increment();
    }
    template <typename U>
    SharedPtr(SharedPtr<U>& other) : mPtr{other.get()} {
        mBlockPtr = other.getBlock();
        mBlockPtr->increment();
    };  // copy ctor
    template <typename U>
    SharedPtr(SharedPtr<U>&& other) : mPtr{other.get()} {
        mBlockPtr = other.getBlock();
    }  // move ctor


    template <typename U>
    SharedPtr operator=(SharedPtr<U>& other) : mPtr{other.get()} {
        mBlockPtr = other.getBlock();
        mBlockPtr->increment();
        return this;
    };  // copy ctor

    template <typename U>
    SharedPtr operator=(SharedPtr<U>&& other) : mPtr{other.get()} {
        mBlockPtr = other.getBlock();
        return this;
    }  // move ctor

    ~SharedPtr() {
        
        if (mPtr != nullptr) {
            mBlockPtr->decrement();
            if (mBlockPtr->refCount() == 0) {
                delete mPtr;
            }
        }
    }

    void swap(SharedPtr<T>& other) {
        T* tmp = this->get();
        ControlBlockBase* tmp2 = this->getBlock();
        mPtr = other.get();
        mBlockPtr = other.getBlock();
        other.set(tmp);
        other.setBlock(tmp2);
    }
    
    void reset() {
        mBlockPtr->decrement();
        mPtr = nullptr;
    }

    void reset(T* newPtr) {
        mBlockPtr->decrement();
        if (mBlockPtr->refCount() == 0 && mPtr != nullptr) {
            delete mPtr;
            delete mBlockPtr;
        }
        mPtr = newPtr;

        mBlockPtr = new ControlBlock(newPtr);
    }

    ControlBlockBase* getBlock() { return mBlockPtr; }
    void setBlock(ControlBlockBase* ptr) { mBlockPtr = ptr; }


    T& operator*() const { return *mPtr; }
    T& operator->() const { return mPtr; }
    T* get() const { return mPtr; }
    void set(T* ptr) { mPtr = ptr; }

    bool operator==(const SharedPtr<T>& other) const { return other.get() == mPtr; }
    bool operator!=(const SharedPtr<T>& other) const { return !(other.get() == mPtr); }

    operator bool() const { return mPtr == NULL; }

    long useCount() { return mBlockPtr->refCount(); }



private:
    T* release() {
        T* tmp = mPtr;
        mPtr = nullptr;
        return tmp;
    }

    T* mPtr;
    ControlBlockBase* mBlockPtr = nullptr;
};

template <typename T, typename... Args>
SharedPtr<T> makeShared(Args&&... args) {
    return SharedPtr<T>(new T(std::forward<Args>(args)...));
}

#endif
