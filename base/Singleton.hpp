#pragma once
/*
    单例设计模式，
    Singleton 模板确保了类 T 只会被实例化一次，
    且这个实例可以在程序的任何地方被访问。
*/

template<typename T> class Singleton{
private:
    //储存单例类的唯一实例对象
    //static意味着value_与类关联，而不是某一对象关联
    static T* value_ ;
public:
    static T& Instance(){
        //pthread_once(&ponce_, &Singleton::init);
		if (nullptr == value_)
		{
			value_ = new T();
		}
		return *value_;
    }

private:
    Singleton();
    ~Singleton() = default;

    //显式地删除其他俩种默认构造函数
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    static void init()
	{
		value_ = new T();
		//::atexit(destroy);
	}

    static void destroy()
	{
		delete value_;
	}


};

//类的静态成员变量要在类外初始化。
template<typename T>
T* Singleton<T>::value_ = nullptr;