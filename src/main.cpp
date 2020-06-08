#include <iostream>
#include <vector>
#include <string>
#include <smart_ptr.hpp>
#include <memory>
#include "log.hpp"

struct Object
{
    Object() : data(0)
    {

    }

    virtual ~Object()
    {
        std::cout << __func__ << std::endl;
    }

    int data;
};

class ObjectExt : public Object
{
public:
    ObjectExt()
    {
    }

    ~ObjectExt()
    {
        std::cout << __func__ << std::endl;
    }

private:

};


int main()
{
    struct OnExit
    {
        OnExit()
        {
            LOG("enter main");
        }

        ~OnExit()
        {
            LOG("exit main");
        }
    } onExit;

    shared_ptr<Object> ptr = make_shared<Object>();

    //ptr.show();

    //ptr->data++;
    //std::cout << ptr->data << std::endl;
    //{
    //    //printf("%s:%d shared: %d weak: %d\n");
    //    ptr.show(__LINE__);
    //    shared_ptr<Object> other = ptr;
    //}
    //std::shared_ptr<int> sp;
    //sp.reset()

    {
        shared_ptr<Object> obj = shared_ptr<Object>(new ObjectExt(),
            [](Object* o)
        {
            delete dynamic_cast<ObjectExt*>(o);
            std::cout << "deleter!" << std::endl;
        });
    }

    return 0;
}
