#include <vector>


template<class T>
struct Handler
{
    Handler() { v.resize(4096); }
    ~Handler()
    {
	for(unsigned int i = 0; i < v.size(); i++)
	    delete v[i];
    }
    std::vector<T *> v;
};

// jb: collects the memory allocated by exprs and frees it
template<class T>
static void Handle_memory(T *t)
{
    // static objects are deleted last
    static Handler<T> h;

    h.v.push_back(t);
}

